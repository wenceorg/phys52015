---
title: "OpenMP: stencils"
weight: 5
katex: true
---

# Parallelisation of a simple stencil

We'll be running these exercises on Hamilton or COSMA, so remind
yourself of how to log in and transfer code [if you need to]({{< ref
hamilton-quickstart.md >}}).


## Blurring an image

One can blur or smooth the edges of an image by convolving the image
with a [normalised box](https://en.wikipedia.org/wiki/Box_blur)
kernel. Every output pixel \\( g_{k, l} \\) is created from the mean
of the input image pixel \\(f _{k, l}\\) and its eight neighbours.

$$
g_{k,l} = \frac{1}{9} \begin{bmatrix} 1 & 1 & 1\\\ 1 & 1 & 1\\\ 1&1&1
\end{bmatrix} * f_{k, l}
$$

This can be implemented by a loop over every pixel of the image,
accessing some a small _stencil_ of data.

This computational pattern appears in both image processing and finite
difference discretisations of partial differential equations (there is
more on the computational aspects of this in
[COMP52315](https://teaching.wence.uk/comp52315), and the numerics in
[COMP52215](https://www.dur.ac.uk/postgraduate.modules/module_description/?year=2021&module_code=COMP52215)
if you're interested).

The code for this exercise is provided as a [tar archive]({{< code-ref
blur_image.tgz >}}).

{{< hint info >}}

You can also clone the [entire course repository]({{< repo >}}), which
gives you access to the code for all the course exercises in the `code`
subdirectory.

{{< /hint >}}

Download and unpack it with

```sh
$ wget {{< code-ref blur_image.tgz >}}
$ tar zxvf blur_image.tgz
```

This will create a new directory `blur_image` with three
subdirectories.

```sh
$ cd blur_image
$ ls
images openmp vec
```

The `images` directory contains images in
[PPM](https://en.wikipedia.org/wiki/Netpbm) format for that will serve
as input to our program. We're going to be working in the `openmp` subdirectory.

```sh
$ cd openmp
$ ls
Makefile  filters.c io.c      main.c    proto.h
```

There is some source code and a
[`Makefile`](https://www.gnu.org/software/make/) that provides a
recipe for how to build the executable. It is just a text file and can
be edited with your favourite text editor. By running `make` you build
the executable.

Before we do this, we'll have to load the right compiler modules.
We'll use the intel compiler for this exercise, since it produces
better reports than gcc.

{{< tabs compiler-modules >}}
{{< tab Hamilton >}}
```sh
intel/xe_2018.2
gcc/9.3.0
```
{{< /tab >}}
{{< tab COSMA >}}
```sh
intel_comp/2018
```
{{< /tab >}}
{{< /tabs >}}

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
$n=1$.

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
