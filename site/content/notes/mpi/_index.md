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

## Concepts

The message passing model is based on the notion of processes (rather
than the threads in OpenMP). Processes are very similar, but do not
share an address space (and therefore do not share memory).

{{< columns >}}

{{< manfig
    src="shared-memory-sketch.svg"
    width="100%"
    caption="Threads can share memory." >}}

<--->

{{< manfig
    src="processes-private-memory-sketch.svg"
    width="100%"
    caption="Processes do not share memory." >}}

{{< /columns >}}

Like in OpenMP programming, we achieve parallelism by having many
processes cooperate to solve the same task. We must come up with some
way of dividing the data and computation between the processes.

Since processes do not share memory, they must instead send messages
to communicate information. This is implemented in MPI through library
calls that we can make from our sequential programming language.
This is in contrast to OpenMP which defines pragma-based extensions to
the language.

The core difficulty in writing message-passing programs is the
conceptual model. This is a very different model to that required for
sequential programs. Becoming comfortable with this model is key to
understanding MPI programs. A key idea, which is different from the
examples we've seen with OpenMP, is that there is much more focus on
the _decomposition of the data and work_. That is, we must think about
how we divide the data (and hence work) in our parallel program. I
will endeavour to emphasise this through the examples and exposition
when we encounter MPI functions.


