---
title: "Coursework: parallel dense linear algebra"
draft: false
weight: 4
katex: true
---

# Dense linear algebra in parallel

{{< hint warning >}}
You should submit your work for this coursework via DUO. The
submission deadline is 2021/01/11 at 14:00UTC.
{{< /hint >}}

{{< hint info >}}
### Updates 2021/01/05

To avoid OpenBLAS parallelising with threads (and therefore destroying
scaling behaviour), you need to do
```
export OMP_NUM_THREADS=1
export OPENBLAS_NUM_THREADS=1
```

In your submission scripts (before running the benchmark). Apologies
for not noticing this earlier.

### Updates 2021/01/04

Textual benchmark output advertised the wrong order of timing data. It
is `Min, Max, Mean, Standard deviation`. The JSON output is correct.
Update the `bench.c` file to correct this error.

### Updates 2020/12/11

Added link to descriptors for written work in [mark scheme]({{< ref
"#mark-scheme" >}}).

### Updates 2020/12/08

The notation
$$
\left\lfloor \frac{N}{p} \right \rfloor = \frac{N}{p}
$$
means that the floor of the division (i.e. rounding down to the
nearest integer) should be equal to the original division. That is $N$
should be divisible by $p$ with no remainder.

Changed instances of `mpiexec` in the examples to `mpirun` for
consistency with the rest of the notes.

{{< /hint >}}

In this coursework you are to write parallel implementations of some
dense linear algebra operations using MPI, and benchmark the scaling
performance. You should not use other forms of parallelism.

I provide template code that contains input/output routines and
skeleton data structures. There are a number of functions that you
need to provide an implementation for.

You will implement a (simplified) case of a small number of the
parallel [BLAS](https://www.netlib.org/blas/index.html) routines using
MPI.

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
every local piece is the same size. That is, we require, for \\(p\\)
processes and an \\( N \times N \\) matrix that
$$
\lfloor \sqrt{p}\rfloor = \sqrt{p}
$$
and
$$
\left\lfloor \frac{N}{\sqrt{p}} \right\rfloor = \frac{N}{\sqrt{p}}.
$$
This ensures that all processes have the same local matrix size, which
simplifies some implementation.

Furthermore, for matrix-vector products, we also require that
$$
\left \lfloor\frac{N}{p}\right\rfloor = \frac{N}{p}.
$$
The code checks these conditions and produces an error message if they
are not satisfied.

On each process, the local part of
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

Download it as a [tarball here]({{< code-ref "coursework.tgz" >}}),
or else find the code in the `/code/coursework` subdirectory of the [course
repository]({{< repo >}}).

You should do your implementation in the `solution.c` file, which
contains three functions that you need to implement. They are correct
when run in serial, but do not yet work correctly in parallel.

{{< hint info >}}
### Required modules
Building on Hamilton or COSMA needs a few modules. On your own system
you may need to edit the `Makefile`.
{{< tabs "Modules" >}}
{{< tab "Hamilton" >}}
```
$ module load intel/2019.5
$ module load gcc/8.2.0
$ module load openblas
$ module load intelmpi/intel/2019.6
```
{{< /tab >}}
{{< tab "COSMA" >}}
```
$ module load intel_comp/2020
$ module load intel_mpi/2020
$ module load openblas
```
{{< /tab >}}
{{< /tabs >}}
{{< /hint >}}

{{< hint warning >}}

The local matrix-vector and matrix-matrix multiplications are done
with OpenBLAS (hence needing to load the `openblas` module). On
Hamilton at least this library automatically uses thread-parallelism
unless you tell it not to. This will get in the way of the MPI
parallelism we're using here.

To turn it off, ensure that you do

```
export OMP_NUM_THREADS=1
export OPENBLAS_NUM_THREADS=1
```
In your batch submission scripts.

{{< /hint >}}

Having set up the environment by loading the required modules, build
the code with `make` and run the executable with `./main`, with no
arguments, it reports usage information.

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
$ mpirun -n 1 ./main -t CHECK_MAT_MULT -N 8
[MatMult]: TODO, please implement me.
CheckMatMult succeeded.
```

```
$ mpirun -n 4 ./main -t CHECK_MAT_MULT -N 8
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

### Code details

The data types are defined in `mat.h` and `vec.h` for the `Mat` and
`Vec` types respectively.

The `Mat` data type is a pointer to a struct
```c
struct _p_Mat {
  MPI_Comm comm;                /* communicator */
  int n, N;                     /* local and global size */
  int np;                       /* process grid np x np */
  double *data;                 /* matrix entries in row-major format */
};
```
So if you have a `Mat A`, its local size is `A->n`.

Similarly, the `Vec` data type is a pointer to a struct
```c
struct _p_Vec {
  MPI_Comm comm;                /* communicator */
  int n, N;                     /* local and global size */
  int np;                       /* number of processes */
  double *data;                 /* vector entries */
};
```

All routines return an error code (`return 0` for no error) can be
checked with the `CHKERR` macro as shown in the template code.

For debugging purposes, I provide routines

```c
int MatView(Mat A, FILE *output);
int VecView(Vec x, FILE *output);
```

which can be used to print the provided matrix or vector. You can pass
`NULL` to print to standard output.

## Task

You should implement matrix-vector multiplication, and two algorithms
for parallel matrix-matrix multiplication in parallel. You should use
MPI for parallelisation.


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

For this routine, you should implement two approaches (which you
should describe in the report):

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


## Mark scheme and submission {#mark-scheme}

You should submit, via DUO, a **PDF** of your report (max 4 pages),
and a zip-file of your implementation (`solution.c`).

I will build and test your code using the coursework skeleton code, so
you should make sure that your code compiles and runs correctly with
an _unmodified_ version of the skeleton.

| Artifact | Descriptor                                              | Marks |
|---------:|:--------------------------------------------------------|-------|
|     Code | Correct implementation of MatMult                       | 10    |
|     Code | Correct implementation of CANNON                        | 15    |
|     Code | Correct implementation of SUMMA                         | 15    |
|     Code | Code compiles with no warnings and does not leak memory | 15    |
|   Report | Description of algorithms                               | 25    |
|   Report | Analysis and presentation of benchmarking data          | 25    |

The report will be marked with reference to the descriptors for
written work (non-dissertation) found on
[DUO](https://duo.dur.ac.uk/webapps/blackboard/content/listContent.jsp?course_id=_91410_1&content_id=_5831035_1),
see _MSc in Scientific Computing and Data Analysis Programme →
Assessments and Procedures → Marking criteria for written work
(non-dissertation)_.


You can check memory leaks with [Valgrind](https://www.valgrind.org).
I provide some instructions [below]({{< ref "#valgrind" >}}).

## Hints

I find it very helpful to draw things out for a small matrix and small
number of processes. Then I can see what is going on, and figure out
how to generalise things.

For debugging purposes, feel free to add `printf` and other statements
to your code, but your final submission should not contain any
`printf` output.

### Using valgrind for memory correctness {#valgrind}

[Valgrind](https://www.valgrind.org) is an excellent memory debugger.
On Hamilton, you can use it like this:

```
module load valgrind
```

You then run your program with

```sh
$ valgrind --leak-check=full ./main ANY ARGUMENTS HERE
```

When running in parallel you should run one valgrind process per MPI
task. In that case, it is useful to send the output to a log file per
process.

```sh
$ mpirun -n 4 valgrind --log-file="log-file.%p" --leak-check=full ./main ANY ARGUMENTS HERE
```

Unfortunately, the Intel MPI library on Hamilton has a number of
memory leaks. I therefore recommend the following recipe for checking
memory leaks. This is what I will do to check things.

{{< hint warning >}}
**DO NOT** use the MPICH module for any performance testing, since it
does not use the high-speed interconnect!
{{< /hint >}}

```sh
$ module load intel/2019.5
$ module load mpich2
$ module load valgrind
$ module load gcc/8.2.0
$ module load openblas
$ make clean
$ make all
$ mpirun -n 4 valgrind --log-file="valgrind-log.%p" --leak-check=full --suppressions=valgrind.supp ./main ANY ARGUMENTS HERE
```
Where the file `valgrind.supp` is included in the coursework tarball.

A successful run will look something like
```
$ mpirun -n 1 valgrind --leak-check=full --suppressions=valgrind.supp ./main -t CHECK_MAT_MULT -N 8
==28983== Memcheck, a memory error detector
==28983== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==28983== Using Valgrind-3.14.0 and LibVEX; rerun with -h for copyright info
==28983== Command: ./main -t CHECK_MAT_MULT -N 8
==28983== 
CheckMatMult succeeded.
==28983== 
==28983== HEAP SUMMARY:
==28983==     in use at exit: 0 bytes in 0 blocks
==28983==   total heap usage: 241 allocs, 241 frees, 4,310,004 bytes allocated
==28983== 
==28983== All heap blocks were freed -- no leaks are possible
==28983== 
==28983== For counts of detected and suppressed errors, rerun with: -v
==28983== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 2 from 2)
```
Whereas if we failed to deallocate some memory, we see something like
```
==30038== Memcheck, a memory error detector
==30038== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==30038== Using Valgrind-3.14.0 and LibVEX; rerun with -h for copyright info
==30038== Command: ./main -t CHECK_MAT_MULT -N 8
==30038== 
CheckMatMult succeeded.
==30038== 
==30038== HEAP SUMMARY:
==30038==     in use at exit: 64 bytes in 1 blocks
==30038==   total heap usage: 241 allocs, 240 frees, 4,310,004 bytes allocated
==30038== 
==30038== 64 bytes in 1 blocks are definitely lost in loss record 1 of 1
==30038==    at 0x4C29E83: malloc (vg_replace_malloc.c:299)
==30038==    by 0x406474: MatMult (solution.c:23)
==30038==    by 0x404F88: CheckMatMult (check.c:24)
==30038==    by 0x403AF9: main (main.c:148)
==30038== 
==30038== LEAK SUMMARY:
==30038==    definitely lost: 64 bytes in 1 blocks
==30038==    indirectly lost: 0 bytes in 0 blocks
==30038==      possibly lost: 0 bytes in 0 blocks
==30038==    still reachable: 0 bytes in 0 blocks
==30038==         suppressed: 0 bytes in 0 blocks
==30038== 
==30038== For counts of detected and suppressed errors, rerun with: -v
==30038== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 2 from 2)
```

This tells us that memory allocated on line 23 of `solution.c` was
never `free`d.
