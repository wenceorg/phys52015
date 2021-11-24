---
title: "Parallel Hello World"
weight: 1
katex: false
---

# Hello, World!

As with every programming course, the first thing we will do is
compile and run a "Hello world" program. Actually we'll do three. The
goal of this is to familiarise you with the module system on Hamilton,
as well as how to compile code. So take a look at the [quickstart
guide]({{< ref "hamilton-quickstart.md" >}}) if you haven't already.

## A serial version

Log in to Hamilton/COSMA load the relevant compiler modules

{{< tabs >}}
{{< tab Hamilton >}}
```sh
intel/xe_2018.2
gcc/9.3.0
```
{{< /tab >}}
{{< tab COSMA >}}
```sh
intel_comp/2018
```
{{< /tab >}}
{{< /tabs >}}

{{< hint info >}}
For the rest of description, I'll just write "Hamilton", but you
should interpret that to mean Hamilton or COSMA depending on which
you're using. Where there is a particular difference, I'll call it
out.
{{< /hint >}}


Create a C file `serial.c` containing the below.

{{< code-include "hello/serial.c" "c" >}}

{{< hint "info" >}}

All of these code snippets live in the [course repository]({{< repo
>}}), so rather than copying or downloading, you should clone the
repository and work in the relevant `code` subdirectory.

We're working the in the `code/hello` subdirectory for this exercise.

{{< /hint >}}

Having done that you should compile the code with `icc`

```
$ icc -o hello-serial serial.c
```

The creates an executable named `hello-serial` from the `serial.c`
file.

Run it on the login node

```sh
$ ./hello-serial
Hello, World!
```

Next, as you saw in the quickstart guide, we should actually do our
runs on the compute nodes. To do this, we need to create a submission
script

{{< code-include "hello/serial.slurm" "sh" >}}

Since we only have a small job, we have requested the "test" queue.
This has a fast turn-around, but does not allow you to run large
simulations.

{{< hint info >}}
On COSMA you need to select the `cosma` queue (`#SBATCH -p cosma`) and
you'll need to select the right account for charging with `#SBATCH -A
ACCOUNT`. This charging account is one that you should have been told about
when registering, it is used to determine how much time you use.

For many more details, see the [COSMA
documentation](https://www.dur.ac.uk/icc/cosma/support/queues/).
{{< /hint >}}

Submit your job with

```sh
$ sbatch serial.slurm
```

After running, this should create two files named
`hello-serial.SOMENUMBERS.out` and `hello-serial.SOMENUMBERS.err`

```sh
$ ls hello-serial.*.{err,out}
hello-serial.3186773.err  hello-serial.3186773.out
```

The numbers correspond to the job ID. You can see the contents of
these by opening them, or [`cat`](https://linux.die.net/man/1/cat)ing
them to the screen.

The error file should be empty:
```sh
$ cat hello-serial.3186773.err
# No output
```

The output file should contain the string `Hello, World!`
```sh
$ cat hello-serial.3186773.out
Hello, World!
```

## An OpenMP version

One of the parallelisation paradigms we will see in this course is
shared memory parallelism, for which we use
[OpenMP](https://www.openmp.org). OpenMP is a specification for
compiler directives and library routines that can be used to specify
parallelism at a reasonably high level for Fortran, C, and C++
programs.

To compile OpenMP programs, we need the same modules as for the serial
programs above. Our code now looks a little different.

{{< code-include "hello/openmp.c" "c" >}}

If we try and compile this like we did before, we will get an error

```
$ icc -o hello-openmp openmp.c
openmp.c(8): warning #3180: unrecognized OpenMP #pragma
  #pragma omp parallel private(thread) shared(nthread)
          ^

/tmp/iccLQGI6H.o: In function `main':
openmp.c:(.text+0x2a): undefined reference to `omp_get_max_threads'
openmp.c:(.text+0x32): undefined reference to `omp_get_thread_num'
```

First the compiler warns us that it saw a `#pragma` that it did not
recognise. Then the linker complained that there were two undefined
functions.

OpenMP support is implemented in most modern compilers, but has to be
explicitly requested. To do so we must add the additional flag
`-qopenmp` to our compile command

```
$ icc -qopenmp -o hello-openmp openmp.c
```

Our submission script also looks a little different

{{< code-include "hello/openmp.slurm" "sh" >}}

We select the amount of parallelism by setting the `OMP_NUM_THREADS`
environment variable before running the executable.

After submitting our job with `sbatch` we again get some output files
```sh
$ ls hello-openmp.*.{err,out}
hello-openmp.3186885.err  hello-openmp.3186885.out
```
We can inspect the contents as before with `cat`
```sh
$ cat hello-openmp.3186885.out
Hello, World! I am thread 0 of 2
Hello, World! I am thread 1 of 2
```

Note how the individual threads are numbered from zero and live in the
interval [0, 2).

{{< question >}}
Try changing the number of threads. What do you notice about the output?
{{< /question >}}

## An MPI version {#mpi}

The other parallisation paradigm we will use is for programming
_distributed memory_ systems. We will use
[MPI](https://www.mpi-forum.org) for this. MPI is a specification for
a library-based programming model. The standard specifies Fortran
and C/C++ interfaces, and there are wrappers for many popular programming
languages including
[Python](https://mpi4py.readthedocs.io/en/stable/) and
[Julia](https://github.com/JuliaParallel/MPI.jl).

To compile MPI programs, we need to load, in additional to the
previous modules, the right MPI version. So execute

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

in addition to the other module load commands.

Our code again looks different

{{< code-include "hello/mpi.c" "c" >}}

Notice how compared to the OpenMP version, there are no pragmas and
the parallelism is not explicitly annotated, there are just calls to
library functions from MPI.

If we try and compile with `icc`, we get errors.
```
$ icc -o hello-mpi mpi.c
mpi.c(2): catastrophic error: cannot open source file "mpi.h"
  #include <mpi.h>
                  ^

compilation aborted for mpi.c (code 4)
```

To compile this file, we need to tell the compiler about all the
MPI-relevant include files and libraries to link. Since this is
complicated, MPI library implementors typically ship with _compiler
wrappers_ that set the right flags. On Hamilton these are named
`mpicc` (for the MPI wrapper around the C compiler), `mpicxx` (for the
wrapper around the C++ compiler), and `mpif90` (for the Fortran
wrapper). Since we have a C source file, we should use `mpicc`

```
$ mpicc -o hello-mpi mpi.c
```

Running the executable is now also more complicated, we need to use
`mpirun` to launch it. This takes care of allocating parallel
resources and setting up the processes such that they can communicate
with one another.

{{< code-include "hello/mpi.slurm" "sh" >}}

Notice this time how we specified the `par7` queue (you can stick with
the `cosma` queue on COSMA, although you need to change the number of
tasks to 16). You can get information about all the available queues
with `sinfo`. A summary of the types is also available with `sfree`.

{{< hint "info" >}}

On some systems you need to specify the number of processes you want
to use when executing `mpirun`. However, on Hamilton, the metadata in
the scheduler is used to determine the number of processes. Here we
request 1 node and 24 tasks per node.

Hence you should only explicitly specify if you want to run with an
amount of parallelism different to that specified in your submission
script.
{{< /hint >}}

{{< question >}}

Try running on two compute nodes, by changing the `--nodes=1` line to
`--nodes=2`. How many total processes do you now have? What do you
notice about the node names?

{{< /question >}}
