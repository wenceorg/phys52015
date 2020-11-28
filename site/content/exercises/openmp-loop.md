---
title: "OpenMP: parallel loops"
weight: 4
katex: false
---

# Parallelisation of a simple loop

As usual, we'll be running these exercises on Hamilton or COSMA, so
remind yourself of how to log in and transfer code [if you need
to]({{< ref hamilton-quickstart.md >}}).


## Obtaining the code

We're going to use the same `add_numbers` code as we did in the
previous [vectorisation exercise]({{< ref vectorisation-loop.md >}}).
You should undo your edits from that exercise. If you can't remember
what you changed just [download]({{< code-ref add_numbers.tgz >}}) and
unpack the code again.

This time, we'll be working in the `openmp` subdirectory.

{{< details "Working from the repository" >}}

If you cloned the [repository]({{< repo >}}) and have committed your
changes on branches for the previous exercises, just checkout the
`main` branch again and create a new branch for this exercise.

{{< /details >}}

## Parallelising the loop

{{< exercise >}}

Compile and run the code with OpenMP enabled.

Try running with different numbers of threads. Does the runtime
change?

You should use a reasonably large value for `N`.

{{< details Solution >}}

Remember that to compile with OpenMP, we shuold add the appropriate
flag to the compile command, so we need to add `-qopenmp` to the
`CFLAGS` variable in the `Makefile` and then recompile (`make clean`
followed by `make all`).

Having done that, I see no change in runtime, because the code is not
yet parallelised.

{{< /details >}}

{{< /exercise >}}


Check the `add_numbers` routine in `add_numbers.c`. Annotate it with
appropriate OpenMP pragmas to parallelise the loop.

{{< question >}}

Does the code now have different runtimes when using different numbers
of threads?

{{< details Solution >}}

This code can be parallelised using a simple parallel for.

In `add_numbers.c` we annotate the for loop with

```c
#pragma omp parallel for default(none) shared(n_numbers, numbers) reduction(+:result) schedule(static)
```

If I do this, I see that the code now takes less time with fewer
threads.

{{< /details >}}
{{< /question >}}


### Different schedules

Experiment with different [loop schedules]({{< ref
"loop-parallelism.md#loop-schedules" >}}). Which work best? Which work
worst?

{{< exercise >}}

Produce a [strong scaling]({{< ref "scaling-laws.md#amdahl" >}}) plot
for the computation as a function of the number of threads using the
different schedules you investigated.

What do you observe?

{{< details Solution >}}

This is what I get for some different schedules when computing on a
vector of one million numbers, I did not run multiple times to avoid
timing variability.

{{< autofig
    width="75%"
    src="add-numbers-scaling.svg"
    caption="Strong scaling of OpenMP parallelisation of add_numbers with different schedule choices." >}}
{{< /details >}}
{{< /exercise >}}
