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

{{< details Solution >}}

I implemented these approaches in
[`code/openmp-snippets/reduction-different-approaches.c`]({{< code-ref
"code/openmp-snippets/reduction-different-approaches.c" >}}). The
crucial thing to note with the critical section and atomics is to
compute partial dotproducts in private variables and then protect the
single shared update.

That is, do this

```c
#pragma omp for
for ( ... ) {
  mydot += a[i]*b[i];
}
#pragma omp atomic
dotab += mydot;
```

Rather than this

```c
#pragma omp for
for ( ... ) {
#pragma omp atomic
  dotab += a[i]*b[i];
}
```

Since the latter does far more locking and synchronisation than
necessary.

{{< /details >}}

{{< exercise >}}

For each of the approaches, benchmark the time it takes to run the
reduction for a vector with 1 billion entries across a range of
threads, from one up to 48 threads.

Produce plots of the [parallel scaling]({{< ref "scaling-laws.md" >}})
and parallel efficiency of the different approaches.

{{< details Solution >}}

I did this with a vector with 500 million entries, and only ran up to
24 threads, because I didn't want to wait that long, but the results
will be broadly the same.

This time I plot time to solution against number of threads on a
[`semilogy`](https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.semilogy.html)
axis. 

{{< autofig
    src="openmp-reduction-approaches.svg"
    width="50%"
    caption="Time to solution (lower is better) for a vector dot product as a function of number of threads." >}}
    
It looks like all the solutions are broadly identical in performance.

We can also plot the efficiency which tells a similar story

{{< autofig
    src="openmp-reduction-approaches-efficiency.sv"
    width="50%"
    caption="Parallel efficiency (for strong scaling) for a vector dot product." >}}
    
It appears for these benchmarks that the "by hand" approach is
_marginally_ better, but we would have to do more detailed
benchmarking to be confident.

{{< /details >}}
{{< /exercise >}}


{{< question >}}

Which approach works best?

Which approach works worst?

Do you observe perfect scalability? That is, is the speedup linear in
the number of threads?

{{< details Solution >}}

As we see in the plots above, it looks like all the approaches are
basically equally good for this test. The speedup is far from perfect,
this is because the amount of memory bandwidth that the Hamilton
compute node delivers does not scale linearly in the number of cores.
So once we've got to around 8 cores, we're already using all of the
memory bandwidth, and adding more cores doesn't get us the answer
faster.

{{< /details >}}
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

{{< details Solution >}}

I just do this for the reduction clause case, since we determined that
everything is basically the same for the other versions.


I do this (again running with a vector size of 500 million, but
the results will be broadly the same for 1 billion entries I suspect),
and find that when I fill up all the cores, the speedup looks pretty
much identical.

However, at interim thread counts, `OMP_PROC_BIND=spread` produces
better performance.

This is because it places threads on both sockets on the compute node,
maximising the amount of memory bandwidth that the program can obtain.

{{< autofig
    src="openmp-proc-bind-reduction.svg"
    width="50%"
    caption="The reduction performs better when using an intermediate number of threads if we specify a `spread` distribution." >}}
    
{{< /details >}}
{{< /exercise >}}
