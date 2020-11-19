---
title: "Point-to-point messaging in MPI"
weight: 7
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

We will now describe how this works.

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
We will revisit this when we look at [wildcard matching]().

{{< exercise >}}
The code above sends a message from rank 0 to rank 1. Modify it so
that it sends the message from rank 0 to ranks $[1..N]$ when run on
$N$ processes.
{{< /exercise >}}

## When are sends (receives) complete?

`MPI_Send` and `MPI_Recv` are _blocking_, that is, `MPI_Send` does not
complete until the send buffer is available for use again, and
`MPI_Recv` does not complete until the receive buffer is full (all of
the message has been received).

We therefore have to be a little bit careful about deadlocks. If we
send a large message from rank 0 to rank 1 (say), and rank 1 is doing
something else (perhaps also sending a message) such that the receive
does not appear, then the processes can get stuck.

For small messages, we might not observe this phenomenon, since the
MPI library might maintain some internal buffers for copying and
managing in-flight messages. We can see the problem if we consider
code that just exchanges values:

```c
if (rank == 0) {
  MPI_Send(sendbuf, nentries, MPI_INT, 1, 0, ...);
  MPI_Recv(recvbuf, nentries, MPI_INT, 1, 0, ...);
} else if (rank == 1) {
  MPI_Send(sendbuf, nentries, MPI_INT, 0, 0, ...);
  MPI_Recv(recvbuf, nentries, MPI_INT, 0, 0, ...);
}
```

Here both ranks try and send messages to each other before receiving
them. For small messages this often works, but for larger ones it
doesn't.

{{< exercise >}}

The code [`mpi-snippets/ptp-deadlock.c`]({{< code-ref
"mpi-snippets/ptp-deadlock.c" >}}) implements this message passing
deadlock.

It takes one argument, which is the size of message to send. Run it on
two processes. How big can you make this message before you observe a
deadlock? 

{{< /exercise >}}



[^1]: MPI does have some facility for one-sided message passing, but
      we won't cover it in this course.

Motivation

What is it

How does it work

Some API

Point to point basics
