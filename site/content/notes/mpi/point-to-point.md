---
title: "Point-to-point messaging in MPI"
weight: 1
katex: true
---

# Pairwise message exchange

The simplest form of communication in MPI is a pairwise exchange of a
message between two processes.

In MPI, communication via messages is _two-sided_[^1]. That is, for every
message one process sends, there must be a matching receive call by
another process.

{{< manfig
    src="mpi-send-recv-cartoon.svg"
    width="50%"
    caption="Cartoon of sending a message between two processes" >}}
    
We need to fill in some details

1. How will we describe "data"
2. How will we identify processes
3. How will the receiver know which message to put where?
4. What does it mean for a send (or receive) to be complete?

The C function signatures for basic, blocking send and receive are

```c
int MPI_Send(const void *buffer, int count, MPI_Datatype dtype, int dest, int tag, MPI_Comm comm);
int MPI_Recv(void *buffer, int count, MPI_Datatype dtype, int src, int tag, MPI_Comm comm, MPI_Status *status);
```

We first note a few things about the interface, and then describe the
details. All input and output variables are as arguments to the
functions.

{{< hint info >}}

The return value is always an error code. As with normal C
conventions, the return value `0` means that no error occurred. you
can set an error handler in MPI, the default is to abort the program
when an error is encountered. 

Sometimes you will encounter a programming library that does not do
this, so they can handle and provide better error messages. In those
circumstances you should always check the error code.

{{< /hint >}}



Let's look at how this works in more detail.

## Describing the data

To provide the data, we pass a pointer to the start of a buffer we
want to send from (receive into). It's a `void *` so that we can pass
any type. We describe how much data to send (receive) by providing a
`count` and a datatype. MPI datatypes are quite flexible, we will
start off only using builtin datatypes (for describing the basic
variable types that C supports). We show a list of the more common
ones below, see the section [Named Predefined Datatypes C
types](https://www.mpi-forum.org/docs/mpi-3.1/mpi31-report/node459.htm#Node459)
in the MPI standard for the full list.

| C type   | MPI_Datatype |
|:---------|:-------------|
| `char`   | MPI_CHAR     |
| `int`    | MPI_INT      |
| `float`  | MPI_FLOAT    |
| `double` | MPI_DOUBLE   |

For example, to send a single double we would write:

```c
double value = ...;
MPI_Send(&value, 1, MPI_DOUBLE, ...);
```

To send the second and third integers from an array of `ints`
```c
int numbers[3] = ...;
/* &(numbers[1]) is the address of the second entry in the array. */
MPI_Send(&(numbers[1]), 2, MPI_INT, ...);
```

Receiving works analogously, so to receive the two integers, this time
into the first two entries of a buffer

```c
int numbers[3] = ...;
/* We could also have written numbers here, since &(numbers[0]) == numbers */
MPI_Recv(&(numbers[0]), 2, MPI_INT, ...);
```

## Identification of processes and distinguishing messages

The concept that groups together processes in an MPI program is a
_communicator_. To identify processes in a send (receive) we use their
`rank` in a particular communicator. As we saw previously, MPI starts
up and provides a communicator that contains all processes, namely
`MPI_COMM_WORLD`.

Suppose I further (for my application) want to distinguish messages
with the same datatype/count arguments. I can use the _tags_ to do so.
A message sent with tag `N` will only be matched by a receive that
also has tag `N`. Often it doesn't matter that much what we use as a
tag, because we arrange our code so that they are not important.

So if I want to send to rank 1 in `MPI_COMM_WORLD`, I write

```c
MPI_Send(..., 1 /* dest */, 0 /* tag */, MPI_COMM_WORLD);
```

Rank 1 can receive this message with:

```c
MPI_Recv(..., 0 /* src */, 0, /* tag */, MPI_COMM_WORLD, ...);
```

{{< hint warning >}}

The count and datatype are **not used** when matching up sends and
receives, it is only the source/destination pair and the tag.

{{< /hint >}}

To decide on the order in which messages are processed, MPI has a rule
that messages with the same source and tag do not "overtake". So if I
have

```c
if (rank == 0) {
  MPI_Send(&vala, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
  MPI_Send(&valb, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
} else if (rank == 1) {
  MPI_Recv(&a[0], 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  MPI_Recv(&a[1], 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
```

Then on rank 1, `a[0]` will always contain `vala` and `a[1]` will
always contain `valb`.

Let's look at an example. Suppose we have two processes, and we want
to send a message from rank 0 to rank 1.

{{< code-include "mpi-snippets/send-message.c" "c" >}}

Notice how we used `MPI_STATUS_IGNORE` in the status field in
`MPI_Recv`. The other option is to provide an `MPI_Status` object.
This can be used to find out a little more information about the
message.
```c
MPI_Status status;
...;
MPI_Recv(..., &status);
```
We will revisit this when we look at [wildcard matching]({{< ref
"point-to-point-nb.md#wildcards" >}}).

{{< exercise >}}
The code above sends a message from rank 0 to rank 1. Modify it so
that it sends the message from rank 0 to ranks $[1..N]$ when run on
$N$ processes.
{{< /exercise >}}

## When are sends (receives) complete?

Let us think about how MPI might implement sending a message over a
network. One option is that MPI copies the user data to be sent into a
buffer, sends it over the network into another buffer, and then copies
it out into the user-level receive buffer. This is shown in the figure
below.

{{< manfig
    src="mpi-send-recv-with-buffers.svg"
    width="50%"
    caption="Send-receive pair with MPI-provided buffers." >}}
    
To avoid this copy, we would like to directly send through the network

{{< manfig
    src="mpi-send-recv-no-buffer.svg"
    width="50%"
    caption="Send-receive with no buffers." >}}
    
For this to be possible, the send has to wait for the receive to be
available. MPI provides us with sending modes that support both of
these mechanisms.

## Different types of send calls

### Synchronous send: `MPI_Ssend`

This send mode covers the case with no buffers. The program will wait
inside the [`MPI_Ssend`](https://rookiehpc.com/mpi/docs/mpi_ssend.php) call until the matching receive is ready. The
figure below shows a timeline on two processes.

{{< manfig
    src="mpi-ssend-cartoon.svg"
    width="50%"
    caption="Sketch of synchronous send between two processes." >}}
    
### Buffered send `MPI_Bsend`

This send mode allows the user to provide a buffer for MPI to copy
into. The call to
[`MPI_Bsend`](https://rookiehpc.com/mpi/docs/mpi_bsend.php) will
return as soon as the data are copied into the buffer. If the buffer
is too small, an error occurs.

{{< manfig
    src="mpi-bsend-cartoon.svg"
    width="50%"
    caption="Sketch of buffered send between two processes." >}}

{{< hint info >}}
#### Points to note

The receive `MPI_Recv` is always synchronous: it waits until the
buffer is filled up with the complete received message.

In the `Bsend` case, it the receive is issued on process 1 before
process 0 starts the send, then process 1 waits in the `MPI_Recv`
call.

{{< /hint >}}

### I don't want to decide: `MPI_Send`

Managing send buffers by hand for `Bsend` is somewhat tedious, so MPI
provides a get-out option:
[`MPI_Send`](https://rookiehpc.com/mpi/docs/mpi_send.php).

In `MPI_Send`, the buffer space is provided by the MPI implementation.
If enough buffer space is available for the message, it is used (so
the send behaves like `Bsend` and returns as soon as the copy is
complete). If the buffer is full, then `MPI_Send` turns into
`MPI_Ssend`.

{{< hint warning >}}

You can't rely on any particular size of buffer from the MPI
implementation, so you should really treat `MPI_Send` like `MPI_Ssend`.

{{< /hint >}}

{{< hint info >}}

#### Recommendation

`MPI_Bsend` is really an optimisation that you should apply once you
really want to squeeze the last little bit out of your implementation.

Therefore, I would only worry about `MPI_Send` and `MPI_Ssend`.
`MPI_Ssend` is _less forgiving_ of incorrect code, so I recommend
`MPI_Ssend` to catch any deadlock errors.
{{< /hint >}}

### A concrete example

Let us look at the difference in behaviour between `MPI_Ssend` and
`MPI_Send` to observe how `MPI_Send` can hide deadlocks in some
circumstances.

Remember that `MPI_Send` returns immediately if there is enough buffer
space available, but turns into `MPI_Ssend` when the buffer space runs
out.

Here is a short snippet that illustrates the kind of problematic code.
Rank 0 will send a message to rank 1, and then receive a message from
rank 1. At the same time, rank 1 first sends a message to rank 0.
```c
if (rank == 0) {
  MPI_Send(sendbuf, nentries, MPI_INT, 1, 0, ...);
  MPI_Recv(recvbuf, nentries, MPI_INT, 1, 0, ...);
} else if (rank == 1) {
  MPI_Send(sendbuf, nentries, MPI_INT, 0, 0, ...);
  MPI_Recv(recvbuf, nentries, MPI_INT, 0, 0, ...);
}
```

{{< exercise >}}

The code [`mpi-snippets/ptp-deadlock.c`]({{< code-ref
"mpi-snippets/ptp-deadlock.c" >}}) implements this message passing
deadlock.

It takes one argument, which is the size of message to send. Compile
with

```
mpicc -o ptp-deadlock deadlock.c
```

{{< details Hint >}}
Don't forget to load the [relevant MPI module]({{< ref "hello.md#mpi" >}}).
{{< /details >}}

Run it on two processes. 

How big can you make this message before you observe a
deadlock?

{{< details "Cancelling the process" >}}

If you launched the run interactively, type `Control-c` to quite the
hanging process.

If you used the batch system you can use `scancel` followed by the ID
of the job to cancel the job (or set a short timeout in your slurm script).

{{< /details >}}

Try changing the `MPI_Send` calls to `MPI_Ssend`, is there now any
value of the buffer size that completes successfully?

{{< /exercise >}}

## Avoiding deadlocks

### Pairwise communication: `MPI_Sendrecv`

For simple pairwise communication, like our example of exchanging
messages, MPI offers an function that does the equivalent of executing
a send and a receive _simultaneously_ (avoiding the deadlock problem
of sends coming before receives).

[`MPI_Sendrecv`](https://rookiehpc.com/mpi/docs/mpi_sendrecv.php)
pairs up a send and a receive in one call.

{{< exercise >}}

Rewrite the code of [`mpi-snippets/send-message.c`]({{< code-ref
"mpi-snippets/send-message.c" >}}) to use `MPI_Sendrecv`.

{{< /exercise >}}

### Non-blocking communication

The pairwise send-receive is useful. but not general enough to cover
all point-to-point communication patterns we might encounter. MPI
therefore offers "non-blocking" communication modes that return
immediately and allow us to later test if the message has been
sent/received.

This page is already long enough, so they're described in detail
[separately]({{< ref "point-to-point-nb.md" >}}).

## Summary

MPI has flexible point-to-point messaging. The message contents are
described by a pointer to a buffer (to send from/receive into) along
with a count and datatype.

The source or destination of a message is specified by providing the
communicator and a rank.

Messages can be distinguished by tags. Often don't need them for
simple processes, but can be used in advanced usage, or to make sure
that messages don't accidentally match.

You should now know enough to attempt the exercise [sending messages
in a ring]({{< ref "mpi-ring.md" >}}).

[^1]: MPI does have some facility for one-sided message passing, but
      we won't cover it in this course.
