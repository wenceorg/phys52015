---
title: "MPI"
weight: 4
---

# MPI: a messaging API

Both vectorisation and OpenMP level parallelism are possible when
using a single computer, with a shared memory space.

If we want to solve really big problems, we need to move beyond this
to a situation where we have _distributed memory_. In this situation,
we have multiple computers which are connected with some form of
network. The dominant parallel programming paradigm for this situation
is that of _message passing_.

Where in the OpenMP/shared memory world we can pass information
between threads by writing to some shared memory, in distributed
memory settings we need to explicitly package up and send data between
the different computers involved in the computation.

In the high performance computing world, the dominant library for
message passing is [MPI](https://www.mpi-forum.org/), the Message
Passing Interface.

Let's look at a simple ["Hello, World"]({{< ref "hello.md#mpi" >}})
MPI program, and contrast it with the equivalent OpenMP program.

{{< columns >}}
{{< code-include "hello/mpi.c" "c" >}}

<--->
{{< code-include "hello/openmp.c" "c" >}}

{{< /columns >}}

Our MPI programs always start by initialising MPI with `MPI_Init`.
They must finish by shutting down, with `MPI_Finalize`. Inside, it
seems like there is no explicit parallelism written anywhere.

Motivation

What is it

How does it work

Some API

Point to point basics
