---
title: "MPI: simple collectives"
weight: 10
katex: true
---

# Manipulating matrices with collectives

In this exercise, we'll look at some simple manipulations of matrices
with collective operations.

We'll do this with square matrices, where the number of rows (and
columns) is equal to the number of processes.

We'll implement three routines:

1. printing a matrix by gathering it to a single process and printing
   there;
2. transposing a matrix;
3. multiplying a matrix against a distributed vector.

For a data distribution, we will use a one-dimensional distribution
where each process holds one row.

I provide a template file [`matrix.c`]({{< code-ref
"mpi/collectives/transpose.c" >}}) in the `code/mpi/collectives`
subdirectory.

It allocates some matrices and sets up the values.

{{< exercise >}}

You should
implement the three stubbed-out functions

1. `print_matrix`
2. `transpose_matrix`
3. `matrix_vector_product`

{{< /exercise >}}

I provide a routine to print the vector (so you can see if you've
implemented the matrix-vector product correctly).

## Advanced (optional)

You could think about how to extend these routines to handle the case
where the matrices continue to be square, but now each process holds
$k$ rows, rather than one row. The matrix is now $kP \times kP$ when
running with $P$ processes. For the vector, each process will now hold
$k$ entries.

Gathering the matrix for printing and computing the matrix-vector
product are not much more difficult. Transposing becomes rather
harder, because each process needs to send a non-contiguous block of
the input data. You can either rearrange the data so that you send
contiguous blocks of $k\times k$ entries to each process, or use an
MPI derived datatype (see, for example, the EPCC material
[here](http://www.archer.ac.uk/training/course-material/2019/07/mpi-epcc/slides/L10-derivedtypes.pdf)).

Good luck!
