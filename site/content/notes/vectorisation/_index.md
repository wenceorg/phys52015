---
title: "Vectorisation"
weight: 2
---

# Vectorisation

As we saw briefly when [introducing]({{< ref
"hardware-parallelism.md#instruction-parallelism" >}}), the parallelism
available in supercomputing hardware offers vector units at the lowest
level.

Although the type of things we can typically do with vectorisation are
quite limited (compared to other forms of parallelisation), they allow
us to introduce a number of concepts. Moreover, it is unfortunately
the case that we need to be aware of vectorisation concepts if we want
to write high-performance code.

Vectorisation allows us to realise _loop-level data parallelism_. That is,
if we have code that contains loops that are [data parallel]({{< ref
"concepts.md#data-parallelism" >}}), they are potentially candidates
for vectorisation.

Fortunately, lots of scientific computing relies on numerical kernels
that do exhibit data parallelism (and hence can be vectorised).

## Vectorisable loops

Not all loops that we might write can be vectorised. In fact, the set
of conditions we need our loop to obey is quite strict. At a high
level what is going on is that take our loop iterations and do them,
not one at a time, but in chunks of the _vector width_.

For example, recall our simple addition of two vectors

```c
for (size_t i = 0; i < N; i++)
  c[i] = a[i] + b[i];
```

{{< columns >}}

{{< manfig src="scalaradd.svg"
    width="75%"
    caption="Scalar addition, one element at a time" >}}
    
<--->

{{< manfig src="vectoradd.svg"
    width="75%"
    caption="Vector addition, four elements at a time" >}}

{{< /columns >}}

In this example, we take four iterations of the loop at once and do
them in one go with vector instructions.

It is a little as if we had written our loop body as

```c
for (size_t i = 0; i < N; i += 4) {
  c[i+0] = a[i+0] + b[i+0];
  c[i+1] = a[i+1] + b[i+1];
  c[i+2] = a[i+2] + b[i+2];
  c[i+3] = a[i+3] + b[i+3];
}
```

and now we do all the instructions in this loop body in one go. This
is called _loop unrolling_ (because we unrolled the loop).

{{< details "What if N isn't evenly divisible by 4?" >}}

In the case that N isn't evenly divisible by four, we have to have
some cleanup code afterwards. So we would write something like

```c
/* Main loop body (vectorisable) */
for (size_t i = 0; i < (N/4) * 4; i += 4) {
  c[i+0] = a[i+0] + b[i+0];
  c[i+1] = a[i+1] + b[i+1];
  c[i+2] = a[i+2] + b[i+2];
  c[i+3] = a[i+3] + b[i+3];
}

/* Scalar cleanup code */
for (size_t i = (N/4) * 4; i < N; i++) {
  c[i+0] = a[i+0] + b[i+0];
  c[i+1] = a[i+1] + b[i+1];
  c[i+2] = a[i+2] + b[i+2];
  c[i+3] = a[i+3] + b[i+3];
}
```

{{< /details >}}

OK, so now we have four statements that we can execute simultaneously.
But are we allowed to do so? 

For our loop to be vectorisable, it needs to satisfy six conditions:

1. The loop must be _countable_ (on entry, we need to know how many
   iterations we have);
2. It must be single-entry and single-exit;
3. No branching in the loop body (but see details below);
4. It must be the innermost loop;
5. There must be no function calls (but see details below);
6. There must be no _dependencies_ between loop iterations.

Let's treat these in more details in turn.

### Countable loops

A countable loop is one with no data-dependent exits. For example,
this loop is not countable

```c
for (size_t i = 0; i < N; i++) {
  c[i] = a[i] + b[i];
  /* Break exits the loop */
  if (c[i] > 10) break;
}
```

When we start the loop, we know the value of `N`, but we don't know
the value of the arrays, so we can't determine how many iterations
we're going to execute. If we think of unrolling the loop, we would
have

```c
for (size_t i = 0; i < N; i += 4) {
  c[i+0] = a[i+0] + b[i+0];
  if (c[i+0] > 10) break;
  c[i+1] = a[i+1] + b[i+1];
  if (c[i+1] > 10) break;
  c[i+2] = a[i+2] + b[i+2];
  if (c[i+2] > 10) break;
  c[i+3] = a[i+3] + b[i+3];
  if (c[i+3] > 10) break;
}
```

Remember that vector instructions want to perform these four
summations in one go. But if we did that, would have rearranged the
loop body to be
```c
for (size_t i = 0; i < N; i += 4) {
  c[i+0] = a[i+0] + b[i+0];
  c[i+1] = a[i+1] + b[i+1];
  c[i+2] = a[i+2] + b[i+2];
  c[i+3] = a[i+3] + b[i+3];
  if (c[i+0] > 10) break;
  if (c[i+1] > 10) break;
  if (c[i+2] > 10) break;
  if (c[i+3] > 10) break;
}
```

So we always perform four updates, and then break if any of the
conditions are satisfied. This _does not_ produce the same result as
the sequential loop, so vectorisation is invalid.


{{< exercise >}}
Convince yourself that this transformation (moving the breaks to the
end) is invalid by writing some code.

You could use this python snippet as a start

```python
import numpy as np
a = np.logspace(0, 7, base=2, num=8)
b = np.linspace(1, 8, num=8)
c = np.zeros_like(a)
for i in range(len(a)):
    c[i] = a[i] + b[i]
    if c[i] > 15:
        break
print(c)
```
{{< /exercise >}}

### Single-entry single-exit loops

This condition is actually an implication of the requirement for
countable loops. The single-exit rule means we can't have
data-dependent exits. Single-entry is similar, we can't jump into the
loop from somewhere (but you don't write `goto` anyway, so that's
fine).

### No branching ("straight-line" code)

Since vector instructions carry out the same operation on data from
multiple elements of the original loop, different iterations should
not have different control flow. So `switch` statements are typically
not vectorised. That said, many conditionals can be implemented as
masked assignments, in which case vectorisation _is allowed_. Here is
an example of some code that would be vectorised, despite the
conditionals


```c
for (size_t i = 0; i < N; i++) {
   if (a[i] < 10) {
      c[i] = a[i] + b[i];
   } else {
      c[i] = 2*b[i];
   }
}
```

What happens is that the loop is unrolled and _both_ branches of the
conditional are executed. The result is then combined using a
data-dependent mask.

{{< manfig src="mask-registers.svg"
    width="75%"
    caption="Conditional assignment can be vectorised by using masking to blend results together." >}}


One necessary condition is that there are
no _dependencies_ between the statements in the loop. We can ask
ourselves, if we were to permute the order of statements (for example
by swapping the `i+0` and `i+1` accesses), would we still get the
right answer?

### Innermost loops

Since we need straight-line code, the only loop in a loop nest that
can be vectorised is the innermost loop.

```c {linenos=table}
for (size_t i = 0; i < N; i++) {
  for (size_t j = 0; j < M; j++) {
    ...
  }
}
```

The loop on line 1 cannot be vectorised, but the loop on line 2 can.

{{< hint info >}}
Note that other optimisations might be performed that remove the inner
loop. For example, if `M` were compile-time known and small, the inner
loop over `j` might be completely unrolled, leaving the `i` loop a
candidate for vectorisation.
{{< /hint >}}

### No function calls

If we have a loop that contains a call to another function, it cannot
be vectorised unless we can "see" inside that other function to figure
out what is allowed. The intuition is straightforward, the function
might do anything at all, so unless we know what it is doing, we can't
be sure that it meets all the other conditions.

There are some exceptions. If the function is in the same compilation
unit (file) and is _inlined_ (effectively pasting the function body
into the loop), then the loop may be vectorised. Similarly, builtin
mathematical functions are often recognised by compilers as
vectorisable.

### No data dependencies in the loop

This is the most subtle, but also easy to get an intuition for.
Remember that conceptually the way we vectorise a loop is to _unroll_
it and then execute the resulting loop body statements _in parallel_.
For this to be valid, we must be able to execute the statements in any
order we like.

For example, this loop is not vectorisable

```c
for (size_t = 1; i < N; i++)
  a[i] = a[i-1] + a[i];
```
Since the \\(i\\)th iteration depends on the results of the
\\(i-1\\)th iteration.

{{< exercise >}}

Write out the unrolled loop (unrolling by 4) and convince yourself
that you can't reorder the statements in the loop body while
maintaining the same semantics.

{{< /exercise >}}

This particular loop exhibits a _read-after-write_ dependency, some
times called a _flow_ dependency. These are "true" dependencies and
really inhibit parallelisation. There are also a number of other
types, _write-after-read_ (also called anti-dependencies),
_write-after-write_ (also called output dependencies), and
_read-after-read_ (not really dependencies). As usual,
[wikipedia](https://en.wikipedia.org/wiki/Data_dependency) has a good
summary. For our purposes, _read-after-write_ are the difficult ones
to handle. The others can usually be refactored by introducing some
temporary variables (as discussed in the linked wikipedia article).
Typically, they then reveal a read-after-write dependency.

{{< exercise >}}

Consider vectorising for a 4-wide vector unit (that is, four additions
can be done at once). Do you think the following loop is vectorisable?

```c
for (size_t i = 4; i < N; i++) {
  a[i] = a[i - 4] + a[i];
}
```

Explain your reasoning.
{{< /exercise >}}


Section 4.2 of Intel's [compiler vectorisation
guide](https://software.intel.com/sites/default/files/m/4/8/8/2/a/31848-CompilerAutovectorizationGuide.pdf)
has more details on these dependencies.
