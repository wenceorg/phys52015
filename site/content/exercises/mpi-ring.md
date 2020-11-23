---
title: "MPI: messages round a ring"
weight: 7
katex: true
---

# Sending messages around a ring

In this exercise we'll write a simple form of global reduction. We
will set up the processes in a ring (so each process has a left and
right neighbour) and each process should initialise a buffer to its
rank.

To compute a global summation a simple method is to rotate each piece
of data all the way round the ring: at each step, a process receives
from the left and sends to the right.

This is illustrated for five processes in the figure below. The
accumulation happens in the red boxes, and the message at each step is
shown by the arrow between processes.

{{< manfig
    src="mpi-ring-reduction.svg"
    width="75%"
    caption="Rotating a message around a ring, and accumulating." >}}
    

## Code

I provide a template file [`ring.c`]({{< code-ref "mpi/ring/ring.c"
>}}) in the `code/mpi/ring` subdirectory. It initialises and finalises
MPI and provides a stub `ring_reduce` function which you should
implement.

The initial local value is set to the rank. This gives us a nice way
of checking if the summed value is correct, since on $P$ processes,
the final value should be $P(P-1)/2$.

## Challenge

{{< exercise >}}

Implement the `ring_reduce` function. If you're already comfortable
with non-blocking messages, you can use those, otherwise, you will
probably find
[`MPI_Sendrecv`](https://www.rookiehpc.com/mpi/docs/mpi_sendrecv.php)
useful.

Ensure that your function does not modify the value in the `sendbuf`
argument.

Make sure that your code produces the correct answer, independent of
the number of processes you use.

That is, you should not have to hard-code the total number of
processes anywhere.

{{< /exercise >}}

## Scaling study: adding processes

Having implemented the reduction, we'll now measure how the
performance varies when we increase the number of processes. First,
think about how many messages your program sends in total as a
function of the total number of processes.

{{< question >}}
If each message takes a constant amount of time, what algorithmic
complexity do you expect for your implementation as a function of the
total number of processes $P$?
{{< /question >}}

{{< exercise >}}

Time the performance of your reduction from one to 256 processes on
Hamilton. (You'll need to use the batch system for this!)

Produce a plot of the runtime as a function of $P$. Does it tally with
your expectations?

If the total time if _very_ small, you might need to run the reduction
in a loop (say ten times) to get reasonable measurements.

{{< details Hint >}}

You can time a section of code with
[`MPI_Wtime`](https://rookiehpc.com/mpi/docs/mpi_wtime.php)

```c
double start = MPI_Wtime();
/* some code here */
double end = MPI_Wtime();

double total = end - start;
```
{{< /details >}}

{{< /exercise >}}
