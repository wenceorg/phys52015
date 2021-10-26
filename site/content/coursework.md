---
title: "Coursework: stencils and collectives"
weight: 3
katex: true
---

# Overview

This coursework has two parts. In the first part, you will design and
implement OpenMP parallelisation of a simple stencil code. In the
second part, you will develop a tree-based implementation of
[`MPI_Allreduce`]({{< ref "notes/mpi/collectives.md#reductions" >}}),
and then perform some benchmarking of your implementation against both
the vendor-provided version, and the non-scalable version we [saw in
the exercises]({{< ref "mpi-ring.md" >}}).

You will be assessed by submitting both your code, and brief writeups
of your findings through GitHub classroom. See details [below]({{< ref
"#submission" >}}).

To gain access to the template code, you should [accept the classroom
assignment](https://classroom.github.com/a/DKMRJlH9).


## Part 1: OpenMP

The [Gauss-Seidel](https://en.wikipedia.org/wiki/Gauss–Seidel_method)
method is a building block of many numerical algorithms for the
solution of linear systems of equations.

Given some square matrix $A$ and vector $b$, it can be used to solve
the matrix equation
$$
A x = b.
$$
To do this, we split $A$ into lower and strictly upper triangular
parts:
$$
A = L_* + U
$$
where
$$
L_* = 
\begin{bmatrix}
a_{11} & 0 & \cdots & 0 \\\ 
a_{21} & a_{22} & \cdots & 0 \\\ 
\vdots & \vdots & \ddots & \vdots \\\ 
a_{n1} & \cdots & \cdots & a_{nn}
\end{bmatrix}
\quad
\text{and}
\quad
U = 
\begin{bmatrix}
0 & a_{12} & \cdots & a_{1n} \\\ 
0 & 0 & \cdots & a_{2n} \\\ 
\vdots & \vdots & \ddots & \vdots \\\ 
0 & \cdots & \cdots & 0
\end{bmatrix}.
$$
Starting from some initial guess for $x$, call it $x^{(k)}$, a new guess
is obtained by
$$
x^{(k+1)} = L_*^{-1}(b - U x^{(k)}).
$$
Since $L_*$ is lower-triangular, its inverse can be computed
efficiently by forward substitution and so we have, for each entry of
the solution vector
$$
x_i^{(k+1)} = \frac{1}{a_{ii}}\left(b_i - \sum_{j=1}^{i-1}
a_{ij}x_j^{(k+1)} - \sum_{j=i+1}^n a_{ij} x_j^{(k)}\right).
$$

We will apply Gauss-Seidel iteration to solve the same image
construction problem we saw in the [MPI stencil exercise]({{< ref
"mpi-stencil.md" >}}).

### Task: design and implement an OpenMP parallelisation scheme

The code for this part is in the `openmp` subdirectory of the template
repository. You can build the code with `make` which produces a
`./main` executable. A sample image if provided in `images/`, but any
other PGM image will also work.

You goal is to parallelise the reconstruction of the image in the
`ReconstructFromEdges` function. Take care that your parallelisation
scheme does not suffer from [data races]({{< ref
"notes/openmp/loop-parallelism.md#data-races" >}}). Is there a way you
can parallelise both the loops over `i` and `j`?

{{< hint info >}}

The Gauss-Seidel iteration is order-dependent, so when you run in
parallel you will not obtain exactly the same answers _even with a
correct implementation_.

{{< /hint >}}

{{< details Hint >}}

Think about which entries of the `data` array are read from for a
particular iteration. Is there a way you can split the loops so that
you can run them in parallel without data races? Breaking iterations
into independent sets that can run in parallel is often term
"colouring". You can see a description of this for Gauss-Seidel
iterations in pages 14-16 of [these slides](http://adl.stanford.edu/cme342/Lecture_Notes_files/lecture10-14.pdf).

{{< /details >}}

Make sure that you commit your changes and push them to your GitHub
repository. Ensure that your code compiles without warnings and does
not leak any memory.

### Part 1 writeup

Write up a short description (max one page) of your OpenMP
parallelisation scheme. You should explain what problems arise when
trying to parallelise the loops, and how you solve them. You should
include this writeup in your repository as PDF file called
`part1.pdf`.

## Part 2: MPI

In the one of the [first MPI exercises]({{< ref "mpi-ring.md" >}}), we
implemented a simple collective operation where we passed messages
around a one-dimensional ring.

In this coursework, we're going to implement a more sophisticated
version of this collective operation, using a tree reduction.

We will then benchmark the ring reduction, tree reduction, and the
builtin `MPI_Allreduce` for a range of problem sizes, and compare to the
simple performance models [we saw in lectures]({{< ref
"notes/mpi/collectives.md" >}}).

For this part of the coursework, the template code lives in the `mpi`
subdirectory. The `Makefile` builds an executable `main`.

{{< hint info >}}
On Hamilton you'll want to load the following modules
```
gcc/9.3.0
intelmpi/gcc/2019.6
```
{{< /hint >}}

We can see how to use it by running with the `-h` flag
```
$ make
$ ./main -h
Usage:
./main -N N [-t MODE] [-h]
Run benchmarking or checking of allreduce.

Options:
 -N N
    Set the message count (required).

 -t BENCH | CHECK
    Select execution mode (default CHECK).
    CHECK: non-exhaustive check of correctness.
    BENCH: print timing data.
 -h
    Print this help.
```

In checking mode, your `tree_allreduce` implementation is compared for
correctness against `MPI_Allreduce`, and some output is printed.
Initially all tests will fail because there is no implementation. A
successful test run will look like the below. You should check that
the tests run correctly for a variety of different process counts.

```
$ mpiexec -n 2 ./main -t CHECK
1..16
ok 1 - MPI_SUM count 1
ok 2 - MPI_PROD count 1
ok 3 - MPI_MIN count 1
ok 4 - MPI_MAX count 1
ok 5 - MPI_SUM count 3
ok 6 - MPI_PROD count 3
ok 7 - MPI_MIN count 3
ok 8 - MPI_MAX count 3
ok 9 - MPI_SUM count 12
ok 10 - MPI_PROD count 12
ok 11 - MPI_MIN count 12
ok 12 - MPI_MAX count 12
ok 13 - MPI_SUM count 1048577
ok 14 - MPI_PROD count 1048577
ok 15 - MPI_MIN count 1048577
ok 16 - MPI_MAX count 1048577
```

### Task: implement `tree_allreduce`

The full version of `MPI_Allreduce` can handle arbitrary datatypes and
combining operations (including user operations). For our
implementation, we will restrict to `MPI_INT` data and the builtin
operations `MPI_SUM`, `MPI_PROD`, `MPI_MIN`, and `MPI_MAX`.

We will also restrict ourselves to reductions over communicators whose
size is a power of two (for example 1, 2, 4, 8, ...). 

{{< details "Optional extra" >}}

Extension to arbitrary sized communicators is not conceptually
difficult, but does introduce quite a bit of fiddly book-keeping.

{{< /details >}}

In `reduce.c` is a stub function `tree_allreduce` in which you should
implement the reduction.

Recall, from the introduction to [collectives]({{< ref
"notes/mpi/collectives.md" >}}), the picture of a tree reduction where at
each level pairs of processes produce partial results. To obtain the
final result on all processes, the sequence of messages is reversed
from root to leaf.

Ensure that your code performs correctly, does not leak any memory
(all objects that you allocate in `tree_allreduce` should be freed),
and compiles without warnings.

### Task: benchmarking and comparison to `MPI_Allreduce`

The main executable has some benchmarking options which you can run
that control the algorithm choice for reductions, and the size of the
messages being sent.

{{< hint info >}}

You will need to do the benchmarking on the compute notes of Hamilton,
so you will need to write a batch script and submit your code to the
batch system.

Leave yourself enough time to get your benchmarking runs completed,
since Hamilton often has significant usage.

{{< /hint >}}

If you run in benchmarking mode, it prints out a short timing summary
of the time to perform the reductions.
```
$ mpiexec -n 4 ./main -N 1024 -t BENCH
1024 366972 1.29243e-05 2.84218e-05 2.93392e-06
```
In order, these give

1. Count (number of integers in the reduction)
2. Number of repetitions 
3. Time for one repetition of `ring_allreduce`
4. Time for one repetition of `tree_allreduce`
5. Time for one repetition of `MPI_Allreduce`


Recall from the lectures that we [constructed models]({{< ref
"notes/mpi/collectives.md" >}}) for how ring and tree reductions
should behave in terms of the number of processes. For a reduction
involving $P$ processes, the ring-based scheme takes time

$$
T_\text{ring}(B) = (P-1)T_\text{message}(B)
$$
and the tree-based scheme takes
$$
T_\text{tree}(B) = 2 \log_P T_\text{message}(B),
$$
where $T_\text{message}(B)$ is the time to send a message of $B$ bytes.

Your task is to benchmark the ring and tree-based implementations, as
well as the vendor-provided `MPI_Allreduce`, with a goal of comparing
the results to our simple models.

Benchmark the different implementations on a range of processes, from
2 up to 128, and a range of message counts (from 1 up to $2^{24}$).

{{< details Hint >}}

It makes sense to collect the data for different message counts but a
fixed process count in the same job script. For example, to collect
data for a count of 1, 2, and 4, your job script could execute

```
mpiexec ./main -N 1 -t BENCH
mpiexec ./main -N 2 -t BENCH
mpiexec ./main -N 4 -t BENCH
```

{{< /details >}}


Produce plots of the behaviour of the reductions as a function of the
number of processes, and the size of the messages. Think about how
best to present these data. Compare them with the data we obtained for
ping-pong messages, and the performance models we developed.

### Part 2 writeup

Write up both the algorithm/implementation choices for
`tree_allreduce` and your findings in a short report (max four pages).
In addition to describing the implementation choices, your report
should present the results of your experiments, along with an analysis
of the data. Remember to justify and explain your parameter choices
for the experiments you carried out.

Some questions you could consider include:

1. Which implementation is the best choice? Does it depend on the
   number of processes taking part?
2. What scaling do you observe as a function of the number of
   processes and message size? 
3. Does the scaling behaviour match the predictions of the models we
   constructed in lectures?
4. Do you observe the same algorithmic scaling behaviour for
   `MPI_Allreduce` as for `tree_allreduce`? If not, what do you think
   might be the difference?

You may also cover other points that your noticed or found interesting.

You should include this writeup in your repository as PDF file called
`part2.pdf`.

## Mark scheme and submission {#submission}

You should submit your work by uploading a single text file containing
the git commit hash of the code and writeups you want marked in the
GitHub classroom repository. **Make sure to push your changes to this
repository regularly**.

Your repository should contain your implementations and two **PDF**
reports:

1. `part1.pdf`: Max one page, covering your implementation for Part 1
2. `part2.pdf`: Max four pages, covering your implementation and
   experiments for Part 2

|      Artifact | Descriptor                                                                               | Marks |
|--------------:|:-----------------------------------------------------------------------------------------|-------|
|      All code | Compiles with no warnings and no memory leaks                                            | 5     |
|   Part 1 code | Correct OpenMP parallelisation of Gauss-Seidel iteration                                 | 15    |
|   Part 2 code | Correct MPI implementation for `tree_allreduce`                                          | 20    |
| Part 1 report | Description of parallelisation scheme                                                    | 10    |
| Part 2 report | Description of implementation choices and analysis and presentation of experimental data | 50    |

The reports will be marked with reference to the [descriptors for
written
work](https://durhamuniversity.sharepoint.com/teams/MScScientificComputingandDataAnalysis/SitePages/Written-Work-Descriptors-(Non-Dissertation).aspx)
on the MISCADA sharepoint site.
