---
title: "MPI: Calculating π"
weight: 8
katex: true
---

# Simple MPI parallelism

In this exercise we're going to compute an approximation to the value
of π using a simple Monte Carlo method. We do this by noticing that if
we randomly throw darts at a square, the fraction of the time they
will fall within the incircle approaches π.

Consider a square with side-length \\(2r\\) and an inscribed circle
with radius \\(r\\).

{{< manfig src="square-circle.svg" 
    width="40%"
    caption="Square with inscribed circle" >}}

The ratio of areas is

$$
\frac{A_\text{circle}}{A_\text{square}} = \frac{\pi r^2}{4 r^2} = \frac{\pi}{4}.
$$

If we therefore draw \\(X\\) uniformly at random from the distribution
\\( \mathcal{U}(0, r) \times \mathcal{U}(0, r)\\), then the
probability that \\(X\\) is in the circle is

$$
p_\text{in} = \frac{\pi}{4}.
$$

We can therefore approximate π by picking \\(N_\text{total}\\) random
points and counting the number, \\(N_\text{circle}\\), that fall within the
circle

$$
\pi_\text{numerical} = 4 \frac{N_\text{circle}}{N_\text{total}}
$$

## Obtaining the code

The code for this exercise is again provided as an [archive]({{<
code-ref "calculate_pi.tgz" >}}), so download it an unpack it.

{{< details "Working from the repository" >}}

If you cloned the [repository]({{< repo >}}) and have committed your
changes on branches for the previous exercises, just checkout the
`main` branch again and create a new branch for this exercise.

{{< /details >}}

The code contains two subdirectories. We'll be working in the `serial`
subdirectory.

```sh
$ cd calculate_pi/serial
$ ls
Makefile       calculate_pi.c main.c         proto.h
```

Load the relevant Intel compiler modules and then build the code with
`make`. It can be run with `./calc_pi N` where `N` is the total number
of random points.

## Convergence

{{< exercise >}}

Run the code for different choices of `N` and plot the error as a
function of `N`.

What relationship do you observe between the accuracy of the
approximate result and `N`?

{{< details Hint >}}
You can execute a sequence of commands in your batch script (just make
sure to allocate enough time in the queue for them all), rather than
running each one in its own job.
{{< /details >}}

{{< /exercise >}}

## Parallelisation with MPI

We're now going to parallelise this computation with MPI. The first
thing to do is to load the appropriate module to gain access to the
MPI compilers.

{{< tabs MPI-modules >}}
{{< tab Hamilton >}}
```sh
$ module load intelmpi/intel/2018.2
```
{{< /tab >}}
{{< tab COSMA >}}
```sh
$ module load intel_mpi/2018
```
{{< /tab >}}
{{< /tabs >}}

### Initialising MPI

Remember that the first thing every MPI program should do is to
_initialise_ MPI with
[`MPI_Init`](https://www.mpi-forum.org/docs/mpi-3.1/mpi31-report/node225.htm)
and the final thing they should do is _finalise_ MPI with
[`MPI_Finalize`](https://www.mpi-forum.org/docs/mpi-3.1/mpi31-report/node225.htm).

So do that in the `main` function (in `main.c`).

{{< exercise >}}

Add code in `main` to determine the
[size](https://www.mpi-forum.org/docs/mpi-3.1/mpi31-report/node155.htm)
of `MPI_COMM_WORLD` as well as the
[rank](https://www.mpi-forum.org/docs/mpi-3.1/mpi31-report/node155.htm)
of the current process, and then print the results out on every
process.

Now compile and then run the code with two processes using `mpirun`.
Does what you observe make sense?

{{< /exercise >}}

### Parallelising the random number generation

So far all we've done is take a serial program and parallelise it by
running the same calculations multiple times. This is not very useful.

The first thing to do is to ensure that the different processes use
_different_ random numbers. These are generated using the C standard
library's pseudo-random number generator. The initial state is
_seeded_ in `calculate_pi`. 

{{< exercise >}}
Modify the code so that the seed depends on which process is
generating the random numbers.

{{< details Hint >}}
The `rank` of a process is a unique identifier.
{{< /details >}}

Run again on two processes, do you now see that the results are
different depending on the process?
{{< /exercise >}}

{{< details "Note: parallel random numbers" >}}
The approach we use here does not produce statistically uncorrelated
random number streams. This does not really matter for our didactic
purposes, it just means that the effective number of Monte Carlo
samples is lower than what we specify.

If you need truly independent random number streams, then the
different approaches [described
here](https://bashtage.github.io/randomgen/parallel.html) give more
information on how to achieve it.
{{< /details >}}


### Dividing the work

Now we have different processes using differents seeds we need to
divide up the work such that we take `N` samples in total (rather than
`N` samples on each process).

Modify the `calculate_pi` function such that the samples are
(reasonably) evenly divided between all the processes. After this
you're producing a partial result on every process.

Finally, combine these partial results to produce the full result by
summing the number of points found to be in the circle across all
processes.

{{< details Hint >}}

Remember that you wrote a function in the [ring reduction]({{< ref
"mpi-ring.md" >}}) exercise to add up partial values from all the
processes.

Alternately, you may find the function
[`MPI_Allreduce`](https://rookiehpc.com/mpi/docs/mpi_allreduce.php) useful.
{{< /details >}}

{{< question >}}

Test your code running on 1, 2, 4, 8, and 16 cores. Produce a plot of
the runtime as a function of the number of cores.

What observations can you make?

{{< /question >}}

### Advice when writing MPI programs

It is good practice when using MPI that every function
which is _collective_ explicitly receives as an argument the
communicator.

If you find yourself explicitly referring to one of the default
communicators (e.g. `MPI_COMM_WORLD`) in a function, think about how
to redesign the code so that you pass the communicator as an argument.

For example, rather than writing something like:

```c
void divide_work(int N, int *nlocal)
{
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  /* Hand out evenly sized chunks with the leftover 
   * (N - (N/size)*size) being evenly split between higher-numbered
   * processes */
  *nlocal = N / size + ((N % size) > rank);
}
```

Instead prefer
```c
void divide_work(MPI_Comm comm, int N, int *nlocal)
{
  int size, rank;
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);
  /* Hand out evenly sized chunks with the leftover 
   * (N - (N/size)*size) being evenly split between higher-numbered
   * processes */
  *nlocal = N / size + ((N % size) > rank);
}
```

This way, if your code is changed to use different communicators, it
will work transparently.
