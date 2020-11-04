---
title: "OpenMP: stencils"
weight: 5
katex: false
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

{{< /exercise >}}
