---
title: "MPI: ping-pong latency"
weight: 9
katex: true
---

# Measuring point-to-point message latency with ping-pong

In this exercise we will write a simple code that does a message
ping-pong: sending a message back and forth between two processes.

We can use this to measure both the _latency_ and _bandwidth_ of the
network on our supercomputer. Which are both important measurements
when we're looking at potential parallel performance: they help us to
decide if our code is running slowly because of our bad choices, or
limitations in the hardware.

## A model for the time to send a message

We care about the total time it takes to send a message, our model is
a linear model which has two free parameters:

1. $\alpha$, the message latency, measured in seconds;
2. $\beta$, the network bandwidth, measured in bytes/second.

With this model, the time to send a message with $b$ bytes is

$$
T(b) = \alpha + \beta b
$$

## Implementation

I provide a template in [`mpi/ping-pong/ping-pong.c`]({{< code-ref
"mpi/ping-pong/ping-pong.c" >}}) that you can compile with `mpicc`. It
takes one argument, the size of the message (in bytes) to exchange.

You should implement the `ping_pong` function which should send a
message of the given size from rank 0 to rank 1, after which rank 1
should send the same message back to rank 0. Ensure that the code also
works with more than two processes (all other ranks should just do
nothing).

Add timing around the `ping_pong` call to determine how long it takes
to send these messages.

{{< details Hint >}}

Use [`MPI_Wtime()`](https://rookiehpc.com/mpi/docs/mpi_wtime.php) for
timing.

For small messages you will probably need to do many ping-pong
iterations in a loop to get accurate timings.
{{< /details >}}

## Experiment

{{< exercise >}}

Run your code on the Hamilton compute nodes for a range of messages
sizes from one byte to 1MB.

Produce a plot of the time to send a message as a function of message
size.

Using
[`numpy.polyfit`](https://numpy.org/doc/stable/reference/generated/numpy.polyfit.html)
(or your favourite linear regression scheme), fit our proposed model
to your data.

What values of $\alpha$ and $\beta$ do you get?


{{< /exercise >}}

{{< question >}}

Perform the same experiment, but this time, place the two processes on
_different_ Hamilton compute nodes. Do you observe a difference in the
performance?

To do this, you'll need to write a SLURM batch script that specifies

```sh
# Two nodes
#SBATCH --nodes=2
# One process per node
#SBTACH --ntasks-per-node=1
```
{{< /question >}}

## Advanced: variability

{{< hint info >}}

This section is optional, but possibly interesting.

{{< /hint >}}

One thing that can affect performance of real MPI codes is the message
latency, and particularly if there is any variability. This might be
affected by other processes that happen to be using the network, or
our own code, or operating system level variability. We'll see if we
can observe any on Hamilton.

Modify your code so that rather than just timing many ping-pong
iterations, it records the time for each of the many iterations
separately.

Use this information to compute the mean ping-pong time, along with
the standard deviation and the minimum and maximum times.

{{< details Hint >}}

You can allocate an array for your timing data with

```c
int nrepeats = 1000; /* Or some approriate number */
double *timing = malloc(nrepeats * sizeof(*double));

...;
for (int i = 0; i < nrepeats; i++) {
   start = MPI_Wtime();
   ping_pong(...);
   end = MPI_Wtime();
   timing[i] = end - start;
}
...; /* Compute statistics */

free(timing); /* Don't forget to release the memory! */
```

{{< /details >}}

Produce a plot of these data, using the standard deviation as error
bars and additionally showing the minimum and maximum times as
outliers.

{{< question >}}

What, if any, variability do you observe?

Does it change if you move from a single compute node to two nodes?

{{< /question >}}

### Network contention

Finally, we'll look at whether having more messages "in flight" at
once effects performance.

Rather than running with two processes, you should run with full
compute nodes (24 processes per node).

Modify your ping-pong code so that all ranks participate in pairwise
messaging.

Divide the processes into a "bottom" and "top" half. Suppose we are
using `size` processes in total. Processes with `rank < size/2` are in
the "bottom" half, the remainder are in the "top" half.

A process in the bottom half should send a message to its matching
pair in the top half (`rank + size/2`), that process should then
return the message (to `rank - size/2`).

Again measure time and variability, and produce a plot.

{{< question >}}

Do the results change from previously?

1. When using one compute node?
1. When using two?
1. When using four?

{{< /question >}}
