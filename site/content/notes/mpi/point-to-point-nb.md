---
title: "Non-blocking point-to-point messaging"
katex: false
weight: 2
---

# Non-blocking messages

As well as the blocking point to point messaging we saw [last
time]({{< ref "point-to-point.md" >}}), MPI also offers _non-blocking_
versions.

These functions all return immediately, and provide a "request" object
that we can then either wait for completion with or inspect to check
if the message has been sent/received.

The function signatures for
[`MPI_Isend`](https://rookiehpc.com/mpi/docs/mpi_isend.php) and
[`MPI_Irecv`](https://rookiehpc.com/mpi/docs/mpi_irecv.php) are:

```c
int MPI_Isend(const void *buffer, int count, MPI_Datatype dtype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
int MPI_Irecv(void *buffer, int count, MPI_Datatype dtype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
```

Notice how the send gets an extra output argument (the request), and
the receive loses the `MPI_Status` output argument and gains a request
output argument.


If we have a request, we can check whether the message it corresponds
to has been completed with [`MPI_Test`](https://rookiehpc.com/mpi/docs/mpi_test.php)

```c
int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status);
```

`flag` will be true if the provided request has been completed, and
false otherwise.

If instead we want to wait for completion, we can use
[`MPI_Wait`](https://rookiehpc.com/mpi/docs/mpi_wait.php)

```c
int MPI_Wait(MPI_Request *request, MPI_Status *status);
```

Which waits until the message corresponding to `request` has been
completed.

## Why would you do this?

Non-blocking messages allow us to separate "posting" messages
from when we check if they are completed. One reason to do this is
that MPI libraries often have optimisations to complete sends quickly
if the matching receive already exists.

If I am receiving messages from 10 different processes, if I use
a blocking `MPI_Recv`, then there is only ever one receive ready at
any one time. Conversely if I use `MPI_Irecv`, then all receives will
be ready, and the MPI library can complete them as the matching send
arrives.

It also allows us to simplify programs that exchange many messages if
we're trying to avoid deadlocks. We can just post all sends/receives
at once and then wait, rather than having to arrange that we have a
single send/receive ready at the right time.

Finally, non-blocking communication allows us to (in theory) _overlap_
communication with computation. This can help to improve scaling
performance in some cases.

As you probably saw when doing the [ping-pong]({{< ref
"mpi-ping-pong.md" >}}) exercise, all MPI messages have a non-zero
latency. That means that no matter how small it is, it takes some
time for a message to cross the network. If we use blocking messages,
the best case total time for our simulation is going to be

$$
T_{\text{compute}} + T_{\text{communicate}}
$$

Many scientific computing simulations have compute and communication
parts that can _overlap_. For example, when domain decomposing a mesh
for a parallel PDE solver, most of the computation can be done
without communicating with our neighbours: we only need information
when we're near the edge of our local domain. We can therefore often
split the simulation into phases:

1. Send data to neighbours
1. Compute on local data that doesn't depend on neighbours
1. Receive data from neighbours
1. Compute on remaining local data

If we use non-blocking messages, we can sometimes hide the latency in
steps (1) and (3), so that the total simulation time is now

$$
\max(T_{\text{compute}}, T_{\text{communicate}}) < T_{\text{compute}} + T_{\text{communicate}}
$$

We will look at this concretely in doing the [halo exchange]({{< ref
"mpi-stencil" >}}) exercise.

## Wildcard matching {#wildcards}


## Examples

## Summary
