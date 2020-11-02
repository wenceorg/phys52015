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
threads, we need to synchronise on reading and writing to shared memory.
## Summary


[^1]: Yes, subtraction isn't associative, so doesn't give us a
    [monoid](https://en.wikipedia.org/wiki/Monoid). The behaviour of
    OpenMP is to treat this like `+`, sum all the partial results and
    then multiply by -1 at the end.
    


