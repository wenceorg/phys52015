---
title: "OpenMP: stencils & profiling"
weight: 5
katex: false
---

# Revisiting the stencil exercise

We'll revisit the stencil computation that we used when [looking at
vectorisation]({{< ref vectorisation-stencil.md >}}). This time, we're
going to look at parallelisation with OpenMP, as well as some
profiling tools.


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

## Profiling

This code is very simple, so we can probably read all of it to figure
out where it is spending all its time. For larger codes, it's instead
best to use some kind of profiling to find out where to start looking.

Depending on the amount of detail you need there are a variety of
useful tools. If you have access to the Intel toolchain then
[VTune](https://software.intel.com/content/www/us/en/develop/tools/vtune-profiler.html)
is quite good: we will use this later when looking at OpenMP
performance.

If you want a rough idea with callstack information, we can use
`gprof`. Although this is not a panacea. In many cases [statistical
profiling](https://en.wikipedia.org/wiki/Profiling_(computer_programming)#Statistical_profilers)
is more useful, especially if it records line-by-line information. On
linux you can do this with
[perf](https://perf.wiki.kernel.org/index.php/Main_Page), but it is
not available on Hamilton or COSMA.

{{< task >}}
We will use `gprof` here. It uses a combination of source code
annotation and statistical sampling to generate approximate call
graphs. Add the appropriate `-pg` flags to the `CFLAGS` variable in
the makefile and recompile. Run the code again.
{{< /task >}}

{{< hint info >}}
Reminder, as always, to use the batch system and submit your code to a
compute node. You'll need to produce an appropriate SLURM submission
script for this.
{{< /hint >}}

{{< question >}}
Look at the output from `gprof`. Which function takes the bulk of the
runtime?
{{< /question >}}

## Parallel scaling

The code is annotated with OpenMP `#pragma`s. Remove the profiling
flags from the makefile and add the relevant compiler flags to enabled
OpenMP parallelisation.

{{< task >}}
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
{{< /task >}}

## Profiling with Intel VTune/Amplifier

We're now going to do more detailed profiling using
[VTune](https://software.intel.com/content/www/us/en/develop/tools/vtune-profiler.html).

Load the relevant modules to gain access to VTune. On Hamilton this
comes with the `intel/xe_2018.2` module you loaded to access the
compilers, on COSMA you'll need to load `vtune/2019-update4`.

Use four OpenMP threads for this experiment.

{{< task >}}
Run the code performing an HPC analysis

```sh
$ amplxe-cl -collect hpc-performance ./blur input.ppm output.ppm
```

{{< /task >}}

For this next bit, you'll need to launch a graphical program on the
login node. So that it displays locally, you need to ssh in with _X
forwarding enabled_. This means you need to run `ssh -Y` rather than
just `ssh`.

An alternative is to have the Intel tools available locally on your
system. A student license is available free. You should then transfer
the results of the profiling to your local machine with `scp` and
analyse them there.

{{< question >}}
You can look at the results using the graphical interface
`amplxe-gui`.

What fraction of the runtime does the code spend in the serial and
parallel parts?

What are the main hotspots in the serial and parallel parts?
{{< /question >}}
