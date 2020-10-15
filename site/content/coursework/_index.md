---
title: "Coursework: parallel dense linear algebra"
draft: false
weight: 4
katex: true
---

# Dense linear algebra in parallel

In this coursework you are to write parallel implementations of some
dense linear algebra operations, and benchmark the scaling
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

TODO: add figure.

The process grid must evenly divide the matrix rows and columns, so
every local piece is the same size.

Vectors are distributed by allocating a contiguous chunk of entries to
each process. Again, the number of process much evenly divide the
number entries in the vector.

This is shown below for an 8-element vector and four processes.

## Skeleton code

I provide a skeleton code in C which provides datatypes for vectors
and matrices, some viewing facilities (for debugging), and has various
options for benchmarking performance and testing correctness.

Download it as a [tarball here](TODO link), or else find the code the
`coursework` subdirectory of the [course repository]({{< repo >}}).

You should do your implementation in the `solution.c` file, which
contains three functions that you need to implement. They are correct
when run in serial, but do not yet work correctly in parallel.

Build the code with `make` and run the executable with `./main`.

## Functions to implement

### `MatMult`: matrix-vector product

### `MatMatMult`: matrix-matrix product

SUMMA algorithm

CANNON algorithm

## Benchmarking

Do strong and weak scaling tests for the matrix-matrix multiplication.
