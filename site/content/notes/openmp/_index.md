---
title: "OpenMP"
weight: 3
---

# OpenMP

Recall that one of the levels of parallelism offered by computer
hardware is [shared memory]({{< ref
"hardware-parallelism.md#shared-memory" >}}) parallelism. In this
style of computer architecture, there are _multiple_ CPU cores that
share main memory.

{{< manfig
    src="numa-socket-sketch.svg"
    width="75%"
    caption="Sketch of a shared memory chip with four sockets." >}}
    
In this picture, any CPU can access any of the memory.

Communication, and therefore coordination, between CPUs happens by
reading and writing to this shared memory.

Equally, division of work between CPUs can be arranged by each CPU
deciding which part of the data to work on.

Let us consider a cartoon of how this might work with two CPUs
cooperating to add up all the entries in a vector. The serial code
looks like this

```c
double sum_vector(double *a, int N)
{
  double sum = 0;
  for (int i = 0; i < N; i++)
    sum = sum + a[i];
  return sum;
}
```

In parallel, the obvious thing to do is to divide up the array into
two halves

{{< columns >}}

CPU 0

```c
for (int i = 0; i < N/2; i++)
  sum0 = sum0 + a[i];
```

<--->

CPU 1

```c
for (int i = N/2; i < N; i++)
  sum1 = sum1 + a[i];
```
{{< /columns >}}

These two _partial_ sums must then be merged into a single result.

## Synchronisation and data races {#sync-data-race}

Conceptually this seems very straightforward, however there are some
subtleties that we should be aware of. The major one is to note that
the partial sums write to separate variables (and hence separate
places in memory).

{{< manfig
    src="SM-vector-sum.svg"
    width="75%"
    caption="Two CPUs coordinating to produce partial sums of an array." >}}

So far, everything is great. When we come to produce the final result,
however, we have a (potential) problem.

{{< manfig
    src="SM-vector-sum-write-contention.svg"
    width="75%"
    caption="Two CPUs might race on writing the final answer." >}}

Since we haven't asked for any _synchronisation_ between the CPUs, if
we naively get the individual CPUs to increment the global summation
result by their partial sum, we are not guaranteed to get the correct
answer (namely `sum0 + sum1`). The three possible outcomes for `sum`
are:

1. `sum = sum0 + sum1`, the correct answer, great!
2. `sum = sum0`, oh no.
3. `sum = sum1`, also wrong.

The reason is that to perform the addition both CPUs first load the
current value of `sum` from memory, increment it, and then store their
answer back to main memory. If `sum` is initially zero, then they
might both load zero, add their part, and then write back. Whichever
is "slower" then wins.

This is an example of a _data race_, and is the cause of most bugs in
shared memory programming.

{{< hint info >}}

A **data race** occurs whenever two or more threads access _the same_
memory location, and _at least one_ of those accesses is a write.

{{< /hint >}}

## Programming with shared memory

The operating system level object that facilitates shared memory
programming is a _thread_. Threads are like processes (e.g. your text
editor and web browser are two different processes), but they
additionally can share memory (as well as having private,
thread-specific, memory). Threads logically all belong to the same
program, but they can follow different control-flow through the
program (since they each have their own, private, program counter).

Usually, though not always, we use one thread per physical CPU core.
However, this is not required.

The other concept is that of a _task_, which we can describe as some
piece of computation that can be executed independently of, and hence
potentially in parallel with, other tasks.

As with most parallel programming, the major idea is to figure out how
to distribute the computational work across the available resources.
[OpenMP]({{< ref "intro.md" >}}) provides facilities for doing this
for [loop-based computations]({{< ref
"loop-parallelism.md" >}}).
