---
title: "OpenMP: synchronisation"
weight: 6
---

# Avoiding data-races in updates to shared memory

In this exercise, we'll use the synchronisation constructs we
encountered when looking at [OpenMP collectives]({{< ref
"notes/openmp/collectives.md" >}}) to implement different approaches
to combining the partial sums in a reduction.

We'll then also benchmark the performance of the different approaches
to see if there are any differences

## Template code and benchmarking

In [`openmp-snippets/reduction-template.c`]({{< code-ref
"openmp-snippets/reduction-template.c" >}}) is some code that times
how long it takes to run the reduction.

You can select the length of the vector to compute the dot product of
by passing a size on the commandline. For example, after compiling with

```
$ icc -qopenmp -o reduction-template reduction-template.c
```

You can run, for example, with
```
$ OMP_NUM_THREADS=2 ./reduction-template 1000000
```

The implementation of the parallel reduction is left up to you.

You should implement a correct reduction using the four different
approaches listed in the code:

1. "By hand", using the same kind of approach as in
`openmp-snippets/reduction-hand.c`;
1. Using an atomic directive to protect the shared updates;
1. Using a critical section to protect the shared updates;
1. Using the reduction clause on a parallel loop.

{{< exercise >}}

For each of the approaches, benchmark the time it takes to run the
reduction for a vector with 1 billion entries across a range of
threads, from one up to 48 threads.

Produce plots of the [parallel scaling]({{< ref "scaling-laws.md" >}})
and parallel efficiency of the different approaches.

{{< /exercise >}}


{{< question >}}

Which approach works best?

Which approach works worst?

Do you observe perfect scalability? That is, is the speedup linear in
the number of threads?

{{< /question >}}

## More details: thread placement

The Hamilton compute nodes are dual-socket. That is, they have two
chips in a single motherboard, each with its own memory attached.
Although OpenMP treats this logically as a single piece of shared
memory, the performance of the code depends on where the memory
accessed is relative to where the threads are.

We will now briefly investigate this.

OpenMP exposes some extra environment variables that control where
threads are physically placed on the hardware. We'll look at how these
affect the performance of our reduction.

Use the implementation with the reduction clause, we hope it is the
most efficient!

The relevant environment variables for controlling placement are
`OMP_PROC_BIND` and `OMP_PLACES`. We need to set `OMP_PLACES=cores`

```
$ export OMP_PLACES=cores
```

{{< details Hint >}}
Don't forget to do this in your submission script.
{{< /details >}}

{{< exercise >}}

We'll now look at the difference in scaling performance when using
different values for `OMP_PROC_BIND`.

As before, run with a vector with 1 billion entries, from one to 48
threads, and produce scaling plots.

First, use

```
OMP_PROC_BIND=close
```

This places threads on cores that are physically close to one another,
first filling up the first socket, and then the second.

Compare this to results obtained with

```
OMP_PROC_BIND=spread
```

This spreads threads out between cores, thread0 to the first socket,
thread 1 to the second, and so on. I think this is the default setting
when using the Intel OpenMP library.

Which approach works better? Is there a difference at all?

{{< /exercise >}}
