---
title: "Loop parallelism"
weight: 2
---

# OpenMP loop parallelism

With a parallel region and identification of individual threads, we
can actually parallelise loops "by hand".

Suppose we wish to divide a loop approximately equally between all the
threads, by assigning consecutive blocks of the loop iterations to
consecutive threads.

{{< manfig
    src="loop-chunking.svg"
    width="75%"
    caption="Distribution of 16 loop iterations across five threads." >}}

Notice here that the number of iterations is not evenly divisible by
the number of threads, so we've assigned one extra iteration to
thread0.

The code that distributes a loop in this way looks something like the below.

{{< code-include "openmp-snippets/hand-loop.c" "c" >}}

{{< exercise >}}
Run this code for a number of threads between 1 and 8. Convince
yourself that it correctly allocates all the loop iterations!
{{< /exercise >}}


UGH!

Fortunately, there is a better way[^1].

[^1]: Counterpoint, if you write large libraries using OpenMP you will
    probably end up managing the loop parallelism and distribution by
    hand in this way anyway.

## Worksharing constructs

Suppose we are in a parallel region, to distribute the work in a loop
amongst the thread team, we use the `#pragma omp for` directive.


```c
void foo(/* Some arguments */)
{
#pragma omp parallel default(none) ...
  {
    ...;

    /* Loop to parallelise */
    #pragma omp for
    for (int i = 0; i < N; i++) {
      ...;
    }
  }
}

```

This directive does the job of dividing the loop iterations between
the currently active threads. Without the directive, all threads in
the team would execute all iterations.

{{< hint warning >}} 

For loop parallelism to be allowable, these loops must obey many of
the same constraints we saw when looking [at vectorisation]({{< ref
"vectorisation.md" >}}). Formally, we need the loop to be

```c
for (var = init; var logical_op end; incr_expr)
  ...
```

Where the `logical_op` is one of `<`, `<=`, `>`, or `>=` and
`incr_expr` is an increment expression like `var = var + incr` (or
similar).

We are also not allowed to modify `var` in the loop body.

{{< /hint >}}

The `for` directive takes a number of additional clauses that allow us
to control _how_ the loop iterations are divided between threads.



This pattern of parallel region + loop is so common that there is a
separate directive that encapsulates the two `#pragma omp parallel for`
