---
title: "Collectives"
weight: 3
katex: true
---

# Collective communication

[Point-to-point]({{< ref "point-to-point.md" >}}) messages are
sufficient to write all the parallel algorithms we might want.
However, they might not necessarily be the most efficient.

As motivation, let's think about the time we would expect the [ring
reduction]({{< ref "mpi-ring.md" >}}) you implemented to take as a
function of the number of processes.

Recall from the [ping-pong exercise]({{< ref "mpi-ping-pong.md" >}})
that our model for the length of time it takes to send a message with
$B$ bytes is

$$
T(B) = \alpha + \beta B
$$

To rotate each item all the way around a ring of $P$ processes
requires each process to send $P-1$ messages. Again supposing we
reduce $B$ bytes:

$$
T_\text{ring}(B) = (P-1)(\alpha + \beta B)
$$

So as we add more processes, the time scales linearly.

An alternative is to combine the partial reductions pairwise in a
tree, as shown in the figure below

{{< manfig
    src="mpi-tree-reduce.svg"
    width="75%"
    caption="Tree reduction across 16 processes. At each level processes combine pairwise." >}}

This approach can be generalised to problems where the number of
processes is not a power of two.

To get the result back to all processes, we can just send the combined
final value back up the tree in the same manner. For $P$ processes,
the tree has depth $\log_2 P$ (we divide the number of by two each
time). So now we only send $2 \log_2 P$ messages (up and down), for a
total time of

$$
T_{\text{tree}}(B) = 2\log_2 P (\alpha + \beta B)
$$

As long as $2\log_2 P < (P-1)$ this takes less overall time. We can
solve this numerically to find it should be preferable as long as $P >
6$.

```python
import numpy
import scipy.optimize


# Do Newton to find P such that 2log_2 P - (P-1) = 0
# We know that P = 1 is a solution, so we deflate that away
def f(p):
    return 1/abs(p-1) * (2*numpy.log2(p) - (p - 1))


root = scipy.optimize.newton(f, 2)

print(root)
=> 6.319722355838366
```

{{< autofig
    src="log-tree-minimum-p.svg"
    width="50%"
    caption="Modelled time to compute a reduction using a 1D ring and a tree, as soon as $P > 6$, the tree is faster"  >}}
    

We conclude that some kind of tree-based reduction is superior in
almost all circumstances.

This reduction is an example of a _collective_ operation. Rather than
sending messages pairwise, all processes in a communicator
work together (or collectively) to compute some result.

## What about implementation?

Writing code for a tree-reduction in MPI by hand is actually not too
hard, and you can have a go if you want.

However, writing it generically to handle all datatypes is slightly
fiddly, and moreover, depending on the topology of the underlying
network, other approaches might be more efficient. For example,
[hypercubes](https://en.wikipedia.org/wiki/Hypercube_(communication_pattern))
often offer a superior communication pattern. 

Our conclusion then, is that we probably don't want to implement this
stuff ourselves.

Fortunately, MPI offers a broad range of builtin collective
operations that cover most of the communication patterns we might want
to use in a parallel program. It now falls to the implementor of the
MPI library (i.e. not us) to come up with an efficient implementation. 

{{< hint info >}}

This is perhaps a general pattern that we observe when writing MPI
programs. It is best to see if our communication pattern fits with
something the MPI library implements before rolling our own.

{{< /hint >}}

## Types of collectives

### Barrier: `MPI_Barrier`

We already saw a barrier in OpenMP, MPI similarly has one, named
[`MPI_Barrier`](https://rookiehpc.com/mpi/docs/mpi_barrier.php). 

```c
int MPI_Barrier(MPI_Comm comm);
```

In a barrier, no process may exit the barrier until all have entered.
So we can use this to provide a synchronisation point in a program.
Note that there are no guarantees that all processes leave at the same
time.

{{< hint warning >}}

There are actually very few reasons that a correct MPI code needs a
barrier.

The usual reason to add them is if we want to measure the parallel
performance of some part of the code, and don't want bad load balance
from another part to pollute our measurements.

{{< /hint >}}

### Reductions

These _are_ useful, there are few related ones.

[`MPI_Reduce`](https://rookiehpc.com/mpi/docs/mpi_reduce.php) combines
messages with a provided operation and accumulates the final result on
a specified `root` rank.

```c
int MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
               MPI_Op op, int root, MPI_Comm comm);
```

Every process provides a value in `sendbuf`, the root process (often
rank 0, but not necessarily) provides an output buffer in `recvbuf`.
The count and datatype arguments are the same as for point-to-point
messaging, they describe how the send and receive buffers are to be
interpreted by MPI. The `op` argument tells MPI how to combine values
from different processes.

{{< manfig
    src="mpi-reduce.svg"
    width="50%"
    caption="`MPI_Reduce` combines values with a specified operation and places the result on the `root` process." >}}

Closely related is
[`MPI_Allreduce`](https://rookiehpc.com/mpi/docs/mpi_allreduce.php)
which does the same thing as `MPI_Reduce` but puts the final result on
all processes (so there is no need for a `root` argument.

```c
int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
                  MPI_Op op, MPI_Comm comm);
```

{{< manfig
    src="mpi-allreduce.svg"
    width="50%"
    caption="`MPI_Allreduce` combines values with a specified operation and places the result on all processes." >}}
    
`MPI_Allreduce` is generally more useful than `MPI_Reduce`, since
often we need to make a collective decision based on the result of the
reduction (for example when checking for convergence of a numerical
algorithm). Although it is possible to implement an allreduce using an
`MPI_Reduce` followed by an
[`MPI_Bcast`](https://rookiehpc.com/mpi/docs/mpi_bcast.php), more
efficient algorithms exist.

There are also "prefix reduction" versions,
[`MPI_Scan`](https://rookiehpc.com/mpi/docs/mpi_scan.php) and
[`MPI_Exscan`](https://rookiehpc.com/mpi/docs/mpi_exscan.php) (for
inclusive and exclusive "scans" of the input respectively).

```c
int MPI_Scan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
             MPI_Op op, MPI_Comm comm);
int MPI_Exscan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
               MPI_Op op, MPI_Comm comm);
```

{{< columns >}}

{{< manfig
    src="mpi-scan.svg"
    width="100%"
    caption="`MPI_Scan` computes an inclusive prefix reduction." >}}

<--->

{{< manfig
    src="mpi-exscan.svg"
    width="100%"
    caption="`MPI_Exscan` computes an exclusive prefix reduction, on rank 0 the value in the output buffer is undefined." >}}

{{< /columns >}}

The final missing piece is what this magic `op` argument is. For
combining simple types, we usually can use one of a number of builtin
options given to us by MPI.

| Description    | Operator                                                  | Identity element                   |
|----------------|-----------------------------------------------------------|------------------------------------|
| Addition       | [`MPI_SUM`](https://rookiehpc.com/mpi/docs/mpi_sum.php)   | 0                                  |
| Multiplication | [`MPI_PROD`](https://rookiehpc.com/mpi/docs/mpi_prod.php) | 1                                  |
| Minimum        | [`MPI_MIN`](https://rookiehpc.com/mpi/docs/mpi_min.php)   | Most positive number of given type |
| Maximum        | [`MPI_MAX`](https://rookiehpc.com/mpi/docs/mpi_max.php)   | Most negative number of given type |
| Logical and    | [`MPI_LAND`](https://rookiehpc.com/mpi/docs/mpi_land.php) | True                               |
| Logical or     | [`MPI_LOR`](https://rookiehpc.com/mpi/docs/mpi_lor.php)   | False                              |
| Logical xor    | [`MPI_LXOR`](https://rookiehpc.com/mpi/docs/mpi_lxor.php) | False                              |
| Bitwise and    | [`MPI_BAND`](https://rookiehpc.com/mpi/docs/mpi_band.php) | All bits 1                         |
| Bitwise or     | [`MPI_BOR`](https://rookiehpc.com/mpi/docs/mpi_bor.php)   | All bits 0                         |
| Bitwise xor    | [`MPI_BXOR`](https://rookiehpc.com/mpi/docs/mpi_bxor.php) | All bits 0                         |


If we pass a `count` of more than 1, then the operation is applied to
each entry in turn. So

```c
int send[2] = ...;
int recv[2] = ...;
MPI_Allreduce(send, recv, 2, MPI_INT, MPI_SUM, comm);
```

Produces the sum over all processes of `send[0]` in `recv[0]` and that
of `send[1]` in `recv[1]`.
