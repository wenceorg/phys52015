---
title: "OpenMP: parallel loops"
weight: 4
---

# Parallelisation of a simple loop

As usual, we'll be running these exercises on Hamilton or COSMA, so
remind yourself of how to log in and transfer code [if you need
to]({{< ref hamilton-quickstart.md >}}).



## Obtaining the code

We're going to use the same code as we did in the previous
[vectorisation exercise]({{< ref vectorisation-loop.md >}}). You
should undo your edits from that exercise. If you can't remember what
you changed just [download]({{< code-ref add_numbers.tgz >}}) and
unpack the code again.

As before, we'll be working in the `serial` subdirectory.

{{< details "Working from the repository" >}}

If you cloned the [repository]({{< repo >}}) and have committed your
changes on branches for the previous exercises, just checkout the
`main` branch again and create a new branch for this exercise.

{{< /details >}}

{{< task >}}

Compile and run the code with profiling enabled using `gprof` to
identify the hotspots.

{{< /task >}}

{{< question >}}
Which functions take most of the runtime? 

Which part of the code would you parallelise to speed it up?
{{< /question >}}

{{< task >}}
Having identified the appropriate hotspots, parallelise it using
OpenMP.

{{< details Hint >}}
If you are struggling, there's a solution in the `openmp`
subdirectory, but try and figure out the solution for yourself first.
{{< /details >}}

{{< /task >}}

{{< question >}}
Run your code using four threads. What is the change in the total
runtime?
{{< /question >}}

We'll now use the VTune profiler to gather some more detailed
information. On COSMA you need to load the `vtune` module to gain
access to it.

{{< task >}}
Run the code using four threads performing and HPC analysis

```sh
$ amplxe-cl -collect hpc-performance ./add_numbers
```

Don't forget that you should do this on the compute node!

{{< /task >}}

You can then use the graphical interface `amplxe-gui` to inspect the
profiling report. To do so, you'll need to log in with _X forwarding_
enabled. Run `ssh -Y` rather than just `ssh` to log in.

An alternative is to have the Intel tools available locally on your
system. A student license is available free. You should then transfer
the results of the profiling to your local machine with `scp` and
analyse them there.

{{< question >}}
Which functions are reported as being the main hotspots in the serial
and parallel parts of the code?
{{< /question >}}
