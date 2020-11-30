---
title: "OpenMP: stencils"
weight: 5
katex: true
---

# Revisiting the stencil exercise

We'll revisit the stencil computation that we used when [looking at
vectorisation]({{< ref vectorisation-stencil.md >}}). This time, we're
going to look at parallelisation with OpenMP.


## Obtaining and compiling the code

If you already downloaded the code from the [previous exercise]({{<
ref vectorisation-stencil.md >}}) you already have the code, otherwise
get the [tar archive]({{< code-ref blur_image.tgz >}}) and unpack it.

This time we'll be working in the `openmp` subdirectory:
```sh
$ cd blur_image/openmp/
$ ls
Makefile  filters.c io.c      main.c    proto.h
```

As before, build the executable with `make`.

{{< hint info >}}
Don't forget to load the correct modules first.
{{< /hint >}}

To confirm that everything works, run the code on one of the input
images to blur it.

## Parallelisation

The code is not yet parallelised. You should parallelise the
`blur_mean` function in `filters.c`, using OpenMP.

What kind of parallelisation is appropriate here? What schedule should
you use?

{{< details Solution >}}

The main work is done in a for loop over the output pixels. Since the
output pixels are all independent, we can use a simple `#pragma omp
parallel for` with a static schedule.
[`code/blur_image/openmp/filters-solution.c`]({{< code-ref
"blur_image/openmp/filters-solution.c" >}}) implements this scheme.

{{< /details >}}

{{< hint info >}}

The code reports when it is running with OpenMP enabled. If you do not
see this, even after parallelisation, check that you enabled the
relevant compiler flags in the `Makefile`.

{{< /hint >}}

## Parallel scaling

Having successfully parallelised the loop we'll look at the [parallel
scaling]({{< ref "scaling-laws.md" >}}) of the problem.

{{< question >}}

What type of scaling is the appropriate one to consider here?

{{< details Solution >}}

Since the total amount of work is fixed, [_strong scaling_]({{< ref
"scaling-laws.md#amdahl" >}}) is appropriate. We are interested in how
fast we can produce the final image as we add more processes (to the
same size problem).

{{< /details >}}

{{< /question >}}

{{< exercise >}}
Investigate the parallel scaling by running the code in parallel using
different numbers of threads

{{< details "Hint" >}}
Remember that you control the number of threads by setting the
`OMP_NUM_THREADS` environment variable. Don't forget to do this _in
your submission script_.
{{< /details >}}

{{< tabs threads >}}
{{< tab Hamilton >}}
Use 1, 2, 4, 6, 8, 16, 24, 32, and 48 threads.
{{< /tab >}}
{{< tab COSMA >}}
Use 1, 2, 4, 6, 8, 16, and 32 threads.
{{< /tab >}}
{{< /tabs >}}

Produce a plot of your results comparing the observed speedup to an
ideal (linear speedup). What do you observe?

To make a problem that runs for a reasonable amount of time you
probably need to use the large sample image (`landscape.ppm`). You may
also need to increase the size of the blur filter from the default
`n=1` (edit `main.c` to do this).

{{< details Solution >}}

I used a static schedule. I get slightly different speedup behaviour
with `n=1` to `n=10`, which the graph below shows.

To grab the timing data in a loop, I just did the following

```sh
for n in 1 2 4 6 8 16 24 32 48; do
    OMP_NUM_THREADS=$n ./blur ../images/landscape.ppm output.ppm;
done | grep Blurring | cut -f 2 -d :
```

Then I manually copied and plotted with matplotlib.

{{< autofig
    src="openmp-blur-image-scaling.svg"
    width="50%"
    caption="Strong scaling (speedup) for the OpenMP parallel image blurring code." >}}

Note that with $n=10$, the overall time is much longer than with
$n=1$ (we saw this behaviour in the [vectorisation]({{< ref
"vectorisation-stencil.md" >}}) version of this exercise).

A Hamilton compute node has only 24 cores, so adding more than 24
threads does not help (indeed it harms). For small $n$, more than 8
threads does not really help. I think this is because the memory
bandwidth is maxed out.

{{< /details >}}
{{< /exercise >}}

{{< exercise >}}

Having produce a scaling plot for the [loop schedule]({{< ref
"loop-parallelism.md#loop-schedules" >}}) you selected, try repeating
the experiments with different loop schedules. For example, if you
used a `static` schedule, try with a `dynamic` or `guided` schedule.

What do you observe? 

Can you explain your results thinking about whether the computational
cost is variable depending on which pixel in the image you are
blurring?

{{< details Solution >}}

I run the main loop with `schedule(runtime)` to control the schedule
and do
```sh
for schedule in static static,100 dynamic dynamic,100 guided guided,100; do
    echo $schedule
    for n in 1 2 4 6 8 16 24; do
        OMP_SCHEDULE=$schedule OMP_NUM_THREADS=$n ./blur ../images/landscape.ppm output.ppm;
    done | grep Blurring | cut -f 2 -d :
done
```

This time I only ran with up to 24 threads, since we already
determined that more than that number is not very helpful. I also only
used $n=1$ for this case

I see the following scaling

{{< autofig
    src="openmp-blur-image-schedules.svg"
    width="50%"
    caption="Strong scaling (speedup) for different schedules." >}}

This time it looks like the guided schedule is best, but this might be
misleading, since each line is normalised to itself.

We can replot these data, producing the speedup curve relative to the
best single-thread performance over all schedules. Let's see what that
looks like

{{< autofig
    src="openmp-blur-image-schedules-best.svg"
    width="50%"
    caption="Strong scaling relative to best single-thread schedule (speedup) for different schedules." >}}

In this case, it looks like the static schedule is now a bit worse. I
am not sure exactly what is going on, and one would need to do more
detailed investigation and profiling to find out.

{{< /details >}}
{{< /exercise >}}
