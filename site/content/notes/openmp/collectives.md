---
title: "Collectives"
weight: 5
katex: true
---

# OpenMP collectives

So far we've seen how we can create thread teams using `#pragma omp
parallel` and distribute work in loops between members of the team by
using `#pragma omp for`.

Now we'll look at what we need to do if we need to communicate between
threads.

## Reductions

Remember that the OpenMP programming model allows communication
between threads by using _shared memory_. If some piece of memory is
shared in a parallel region then every thread in the team can both
read and write to it.

Recall also that there is no synchronisation across accesses to shared
memory. Let us consider what this means for computing a vector dot
product

$$
a \cdot b = \sum_i^N a_i b_i
$$

A serial loop to compute this code might look like the following

```c
dot = 0;
for (int i = 0; i < N; i++) {
  dot += a[i]*b[i];
}
```

A naive parallelisation would just annotate the loop with `#pragma omp
for`. 
```c
dot = 0;
#pragma omp parallel for default(none) shared(dot, a, b) schedule(static)
for (int i = 0; i < N; i++) {
  dot += a[i]*b[i];
}
```
However, this has a problem. The shared variable `dot` is
updated by multiple threads, and so we are not guaranteed that all
increments will be seen since there is a [write race]({{< ref
"openmp.md#sync-data-race" >}}) in the increment of `dot`.

{{< hint info >}}

I discuss some [tools]({{< ref "#data-race-tools" >}}) for detecting
data races below.

{{< /hint >}}

We can test this out with the following code

{{< code-include "openmp-snippets/reduction-race.c" "c" >}}

{{< exercise >}}

This example code computes a dot product in serial and then in
parallel. But the parallel version has race conditions.

Try running with different numbers of threads. Do you always get the
correct answer in parallel? Do you always get the same wrong answer?

{{< /exercise >}}

The solution to this problem is to create partial sums on each thread,
and the accumulate them in a thread-safe way. We could do this like so

<a name="reduction-hand"></a>

{{< code-include "openmp-snippets/reduction-hand.c" "c {linenos=table}" >}}

As the comments indicate, all the barriers are quite delicate.

{{< exercise >}}

Run this code, check that it continues to give you the correct answer
independent of the number of threads you use.

Now explore what happens if you accidentally got some of the barriers
wrong.

1. What happens if you add `nowait` to the `single` directive on line
   28?
2. What happens if you add `nowait` to the `for` directive on line 32?
   (Remove the `nowait` from line 28 again!)

{{< /exercise >}}

### Directives to the rescue

We see that writing a collective reduction by hand is possible, but a
bit tedious. OpenMP provides facilities to handle all of the gory
details by adding an extra
[`reduction`](https://computing.llnl.gov/tutorials/openMP/#REDUCTION)
clause to the `for` directive

```c
#pragma omp for schedule(static) reduction(+:dot)
for (int i = 0; i < N; i++) {
  dot += a[i]*b[i];
}
```

This tells OpenMP that `dot` is a reduction variable, and that the
combining operation is `+`. It now becomes the compiler's job to
generate appropriate code for privatising the partial reduction
contributions from different threads and then combining them.

`reduction` clauses have some restrictions on the type of data they
can combine. We need an associative binary operator (so that the
combination can happen in any order) and an identity element for the
operation (so that the compiler can generate the right initial value
for the privatised reduction variables).

Reductions are defined for builtin datatypes (such as `int`, `double`)
and for the combining operations in the table below

| Operation       | Operator | Initialisation                     |
|-----------------|----------|------------------------------------|
| Addition        | `+`      | `0`                                |
| Multiplication  | `*`      | `1`                                |
| Subtraction[^1] | `-`      | `0`                                |
| Minimum         | `min`    | Most positive number of given type |
| Maximum         | `max`    | Most negative number of given type |
| Logical and     | `&&`     | `1`                                |
| Logical or      | `||`     | `0`                                |
| Bitwise and     | `&`      | `~0` (all bits on)                 |
| Bitwise or      | `|`      | `0` (all bits off)                 |


With this, we can rewrite our dot product example

{{< code-include "openmp-snippets/reduction-directive.c" "c" >}}

This is both more succinct, and potentially better optimised. For
example, the compiler might implement a tree reduction (rather than
the linear reduction I coded above).

### Where can you use `reduction` clauses?

You don't need to annotate a loop with a reduction. Suppose that you
have a parallel region where the threads each do a single expensive
operation and then combine the results. A reduction is entirely
appropriate here too.


## Inter-thread synchronisation

Sometimes, we'll want to do something other than just compute a
reduction. In these cases, if we want to pass information between
threads, we need to synchronise on reading and writing to shared
memory.

### Barriers

OpenMP has a number of constructs for this purpose. We've already
implicitly seen one of them, namely barriers. In a parallel region,
[`#pragma omp
barrier`](https://computing.llnl.gov/tutorials/openMP/#BARRIER) can be
used to synchronise all threads in the team.

```c
#pragma omp parallel for shared(a) default(none)
{
  int tid = omp_get_thread_num();
  int nthread = omp_get_num_threads();
  /* Produce some thread-specific data */
  a[tid] = some_computation(tid, ...);
  #pragma omp barrier
  /* Read data from a neighbouring thread */
  b[tid] = a[(tid+1)%nthread] + ...;
}
```

Without the barrier, there is no synchronisation between the writes to
`a` and the reads when updating `b`. We would therefore likely get the
wrong answer.

The barrier ensures that no thread attempts the update to `b` before
all threads have arrived at the barrier (after updating `a`).

{{< hint warning >}}

Either **all** threads must encounter the barrier, or none, otherwise
we get a deadlock.

{{< /hint >}}

{{< code-include "openmp-snippets/bad-barrier.c" "c" >}}

{{< exercise >}}

Try this deadlock situation out with the above code.

Does it run successfully with one thread? What about two or three?

{{< details Hint >}}

To terminate the hanging program, type `Control-c` at the commandline.

{{< /details >}}

{{< /exercise >}}

Recall that often barriers are implicit in worksharing constructs. So
if we are in a parallel region we do not need a barrier between two
`#pragma omp for` directives for synchronisation (because `#pragma omp
for` has an implicit barrier at the end of the loop).

### Critical sections and atomics

Sometimes a barrier synchronisation is a little heavy-handed. For this
OpenMP provides us two further constructions. For protecting _code_
and _variables_ respectively.

The first is the critical section [`#pragma omp
critical`](https://computing.llnl.gov/tutorials/openMP/#CRITICAL).
This directive specifies that a region of code must be executed by
only one thread at a time.

Here's a trivial example, counting the number of threads in a parallel
region (don't do this, use `omp_get_num_threads()`).

```c
int nthread = 0;
#pragma omp parallel default(none) shared(nthread)
{
  #pragma omp critical
  {
    nthread += 1;
  }
}
```

Critical sections are more useful if you're parallelising over a
shared data structure.

For example, consider a shared task stack of work from which we can
pop work and push work. In pseudo-code, this looks approximately like
the below.

```c
stack = ...;              /* Create initial work */
while (1) {
  task = pop(stack);      /* Get the next work item */
  if (task == NULL) {
    break;                /* No work left, exit loop */
  }
  newtask = work(task);   /* Do work, potentially producing a new task */
  if (newtask != NULL) {
    push(newtask, stack); /* If there's a new task, add it to the stack */
  }
}
```

We can parallelise this with OpenMP, but we need to make sure there
are no race conditions when pushing and popping from the shared
`stack` data structure. This can be achieved with `critical` sections

```c
stack = ...;
#pragma omp parallel default(none) shared(stack)
{
  while (1) {
#pragma omp critical modifystack
    {
      task = pop(stack);
    }
    if (task == NULL) {
      break;
    }
    newtask = work(task);
    if (newtask != NULL) {
#pragma omp critical modifystack
      push(newtask, stack);
    }
  }
}
```

Here we protect the modification of the stack by critical sections. 

Points to note:

1. I gave the critical sections an optional name (`modifystack`). All
   critical sections with the _same name_ synchronise. If no name is
   provided this matches any other critical section without a name.
2. We need the critical sections to have the same name for the push and pop
   because both of these sections modify the same shared data
   structure.
3. This probably isn't a very good implementation because threads
   might exit the loop too early (if they pop from an empty stack
   before new work is pushed by another thread).

{{< hint info >}}

Design of high-performance datastructures for these kind of irregular
computations is actually an ongoing area of research. If you're
interested, look at some of the work that the [Galois
team](https://iss.oden.utexas.edu/?p=projects/galois) are doing.

{{< /hint >}}

{{< exercise >}}

Modify the [`reduction-hand.c`]({{< ref "#reduction-hand" >}}) example
to use a critical section to ensure the result is always correct.

{{< /exercise >}}

### Atomics

Even finer-grained than critical sections are [`#pragma omp
atomic`](https://computing.llnl.gov/tutorials/openMP/#ATOMIC)
directives. These can be used to protect (some) updates to shared
variables.

```c
int nthread = 0;
#pragma omp parallel default(none) shared(nthread)
{
  #pragma omp atomic
    nthread += 1;
}
```

An atomic directive protects variables (not code, like critical
sections). In particular, it protects the **write** to the variable on
the left hand side of an assignment. The allowed form is one of

1. `x op= expr` where `op` is one of `+`, `-`, `*`, `/`, `&`, `^`,
   `<<`, or `>>`
2. `x++`, `++x`, `x--`, `--x`.

Note that the evaluation of `expr` in the first form is not protected,
it is _only_ the write to `x` that is protected.

{{< exercise >}}

We can use these synchronisation constructs to implement different
approaches to the reduction example. The [openmp exercise on
reductions]({{< ref "openmp-reduction.md" >}}) does this.

{{< /exercise >}}

## Tools for detecting data races {#data-race-tools}

We need to be careful when writing parallel code that we do not
accidentally introduce race conditions that produce incorrect results.
There are some tools available to help with this. On Linux-based
systems you can use
[helgrind](https://www.valgrind.org/docs/manual/hg-manual.html).

Modern versions of GCC and Clang also offer a [thread sanitizer
mode](https://github.com/google/sanitizers/wiki#threadsanitizer)
enabled with `-fsanitize=thread`. Again, this is transparently
supported on Linux, but seemingly not on MacOS.

Often, thinking hard is your best bet.

{{< hint warning >}}

Sometimes these tools will report _false positives_, and you need to
work a little bit to eliminate them. See [this nice
article](https://medium.com/@joshisameeran/using-tsan-threadsanitizer-and-ways-to-avoid-false-sharing-in-clang-and-gcc-15fae5283ad1)
for more information.

{{< /hint >}}

{{< exercise >}}

Try compiling and running the [`reduction-race.c`]({{< code-ref
"openmp-snippets/reduction-race.c" >}})) example using GCC and thread
sanitizer enabled. Run with two threads, does it help you to
understand the race condition?

On Hamilton load the `gcc/9.3.0` module, on COSMA load the
`gnu_comp/10.2.0` module. You should then compile with

```
$ gcc -fopenmp -g -fsanitize=thread -o race reduction-race.c
```

The `-g` adds debug symbols so that we see line numbers in the error
reports.

{{< /exercise >}}

## Summary

As well as straightforward loop parallelisation, OpenMP also provides
a number of constructs to help with accumulation of results and
synchronisation when updating shared variables. The biggest hammer is
a `barrier`, but these can often be avoided in favour of more
fine-grained directives. The most useful is probably the `reduction`
clause for loops.


[^1]: Yes, subtraction isn't associative, so doesn't give us a
    [monoid](https://en.wikipedia.org/wiki/Monoid). The behaviour of
    OpenMP is to treat this like `+`, sum all the partial results and
    then multiply by -1 at the end.
