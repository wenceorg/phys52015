---
title: "Coursework: parallel dense linear algebra"
draft: false
weight: 4
katex: true
---

# Dense linear algebra in parallel

In this coursework you are to write parallel implementations with MPI
of some dense linear algebra operations, and benchmark the scaling
performance.

I provide a template file that contains input/output routines and
skeleton data structures. There are a number of functions that you
need to provide an implementation for.

You will implement a (simplified) case of a small number of the
parallel BLAS routines using MPI.

In particular, you should implement the matrix-vector product

$$
y \gets A x
$$

and two algorithms for the matrix-matrix product

$$
C \gets A B + C
$$

For square matrices \\(A, B, C \in \mathbb{R}^{N \times N}\\) and
vectors \\(x, y \in \mathbb{R}^N\\).

## Data layout

To simplify things, the setup is restricted to square matrices, that
are distributed using a 2D block layout, where each process stores a
contiguous block of the matrix.

This is shown below for a \\(8 \times 8 \\) matrix and four processes.

{{< manfig src="matrix-block-distribution.svg"
    width="50%"
    caption="Blocked data distribution of a matrix over four processes." >}}

The process grid must evenly divide the matrix rows and columns, so
every local piece is the same size. On each process, the local part of
the matrix is stored in _row major_ format.

{{< details "Row major indexing" >}}

For a local matrix of size \\( n \times n \\), the \\( (i, j) \\)
entry can be obtained with
```c
Aij = A->data[i*A->n + j];
```
{{< /details >}}

Vectors are distributed by allocating a contiguous chunk of entries to
each process. Again, the number of process much evenly divide the
number entries in the vector.

This is shown below for an 8-element vector and four processes.

{{< manfig src="vector-block-distribution.svg"
    width="25%"
    caption="Blocked data distribution of a vector over four processes." >}}


## Skeleton code

I provide a skeleton code in C which provides datatypes for vectors
and matrices, some viewing facilities (for debugging), and has various
options for benchmarking performance and testing correctness.

Download it as a [tarball here](TODO link), or else find the code in the
`/coursework` subdirectory of the [course repository]({{< repo >}}).

You should do your implementation in the `solution.c` file, which
contains three functions that you need to implement. They are correct
when run in serial, but do not yet work correctly in parallel.

Build the code with `make` and run the executable with `./main`, with
no arguments, it reports usage information.

```
$ cd coursework
$ make
$ ./main
Matrix size is a required argument please specify with -N.

Usage:
./main -N N [-a ALGORITHM] [-t MODE] [-f FILE] [-h]
Run benchmarking or checking of matrix-vector or matrix-matrix multiplication.

Options:
 -N N
    Set matrix size (required).

 -a CANNON | SUMMA
    Select algorithm for matrix-matrix multiplication (default SUMMA).

 -t CHECK_MAT_MULT | BENCH_MAT_MULT | CHECK_MAT_MAT_MULT | BENCH_MAT_MAT_MULT
    Select execution mode (default CHECK_MAT_MAT_MULT).
    CHECK_MAT_MULT: check correctness of matrix-vector multiplication.
    BENCH_MAT_MULT: print timing data for matrix-vector multiplication.
    CHECK_MAT_MAT_MULT: check correctness of matrix-matrix multiplication.
    BENCH_MAT_MAT_MULT: print timing data for matrix-matrix multiplication.

 -f FILE
    In benchmarking mode, print timing data to FILE in JSON format.
    WARNING: overwrites output file if it exists.
    Use "-f -" to dump to standard output.

 -h
    Print this help.
```

The executable has a checking mode, which checks that the
implementations of matrix-vector and matrix-matrix multiplication are
correct. Only the serial multiplication is implemented, so it only
reports a passed test when running with one process. It also prints a
warning that nothing is implemented yet.

```
$ mpiexec -n 1 ./main -t CHECK_MAT_MULT -N 8
[MatMult]: TODO, please implement me.
CheckMatMult succeeded.
```

```
$ mpiexec -n 4 ./main -t CHECK_MAT_MULT -N 8
[MatMult]: TODO, please implement me.
[1] CheckMatMult failed at local index 0, expected 26 got 12
[1] CheckMatMult failed at local index 1, expected 26 got 12
[MatMult]: TODO, please implement me.
[2] CheckMatMult failed at local index 0, expected 66 got 12
[2] CheckMatMult failed at local index 1, expected 66 got 12
[MatMult]: TODO, please implement me.
[3] CheckMatMult failed at local index 0, expected 66 got 8
[3] CheckMatMult failed at local index 1, expected 66 got 8
[MatMult]: TODO, please implement me.
[0] CheckMatMult failed at local index 0, expected 26 got 8
[0] CheckMatMult failed at local index 1, expected 26 got 8
CheckMatMult failed.
```

Now we see an error and reports of where a mismatch was observed.

In benchmark mode, the matrices and vectors are filled with random
numbers and then the relevant operation is performed. On successful
completion of the benchmark, the program prints a short summary of the
performance. It can also dump in machine-readable json format to a
specified file with `-f data.json`.

{{< hint warning >}}
When writing to a file, any existing contents are **overwritten**!
{{< /hint >}}

{{< hint info >}}
You can load json files in Python with something like
```python
import json
with open("data.json", "r") as f:
    data = json.load(f)
```
{{< /hint >}}

For example

```
$ ./main -t BENCH_MAT_MAT_MULT -a CANNON -N 1000
[MatMatMultCannon]: TODO, please implement me.
Timing data for MatMatMult[CANNON] on 1 processes, matrix size 1000
All data in seconds. Min, Mean, Max, Standard deviation.
0.0186729 0.0186729 0.0186729 0
```

## Task

You should implement matrix-vector multiplication, and two algorithms
for parallel matrix-matrix multiplication in parallel. Using MPI for
parallelisation.

Having checked the correctness of your implementation, you should
perform a weak and strong scaling study of the three routines. Up to
around 256 processes on Hamilton (or COSMA).

Finally, you should write a short (maximum four pages) report
describing the algorithms as you implemented them and any choices you
made, and discussing your findings from the scaling study.

### Matrix-vector multiplication

For this routine, you are free to choose your own algorithm (which you
should describe in the report). You should aim for an approach which
is memory scalable, in the sense that the amount of memory needed by
each process is related to \\(N/P\\) rather than \\(N\\) (for an \\(N
\times N \\) matrix on \\( P \\) processes).


### Matrix-matrix multiplication

For this routine, you should implement two approaches.

1. [Cannon's
   algorithm](https://en.wikipedia.org/wiki/Cannon%27s_algorithm),
   which is [described
   nicely](https://people.eecs.berkeley.edu/~demmel/cs267/lecture11/lecture11.html#link_5)
   by Jim Demmel.
2. [SUMMA](http://netlib.org/lapack/lawnspdf/lawn96.pdf), which is a
   more general method (although we do not implement the full
   generality here). David Bindel has a nice description in [these
   slides](https://www.cs.cornell.edu/courses/cs5220/2017fa/lec/2017-10-19-slides.pdf)
   [pages 2-8]. Since we are restricting ourselves to a square grid of
   processes, the data transfer steps can always operate on the full
   local matrix (rather than the extra blocking as described in the
   SUMMA paper).


## Mark scheme and submission

You should submit, via DUO, a **PDF** of your report (max 4 pages),
and a zip-file of your implementation (`solution.c`).

I will build and test your code using the coursework skeleton code, so
you should make sure that your code compiles and runs correctly with
an _unmodified_ version of the skeleton.

| Artifact | Descriptor                                           | Marks |
|---------:|:-----------------------------------------------------|-------|
|     Code | Correct implementation of MatMult                    | 10    |
|     Code | Correct implementation of CANNON                     | 15    |
|     Code | Correct implementation of SUMMA                      | 15    |
|     Code | Code compiles with no warnings and is Valgrind-clean | 15    |
|   Report | Description of algorithms                            | 25    |
|   Report | Analysis and presentation of benchmarking data       | 25    |


## Hints

I find it very helpful to draw things out for a small matrix and small
number of processes. Then I can see what is going on, and figure out
how to generalise things.

For debugging purposes, feel free to add `printf` and other statements
to your code, but your final submission should not contain any
`printf` output.
