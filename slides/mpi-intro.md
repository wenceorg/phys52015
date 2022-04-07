---
title: "MPI: beginnings"
---

# MPI: an introduction

<div class="center">

[Introductory notes](/phys52015/notes/mpi/)

</div>

------

## Main idea

- _Processes_ (not threads), with no shared memory
- Explicit communication between processes
- No "special" process

------

## Picture

Threads share memory

![Threads share memory](/phys52015/images/manual/shared-memory-sketch.svg)

Processes don't

![Processes don't share memory](/phys52015/images/manual/processes-private-memory-sketch.svg)

------

- Most [MPI](https://www.mpi-forum.org/) programs are [single program, multiple
  data](https://en.wikipedia.org/wiki/SPMD) (SPMD)
- Every process gets its own copy of _the same_ program
- ... but each process operates on _its own_ data

------

## Hello world
```c [8,12|9,10|11]
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
  int rank, size, len;
  MPI_Comm comm;
  MPI_Init(&argv, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  printf("Hello, World! [%d/%d]\n", rank, size);
  MPI_Finalize();
  return 0;
}
```

------

## On two processes

<div class="container">

<div class="col">

### Process zero

```c [6,7]
int main(int argc, char **argv)
{
  int rank, size, len;
  MPI_Comm comm;
  MPI_Init(&argv, &argv);
  rank = 0;
  size = 2;
  printf("Hello, World! [%d/%d]\n",
         rank, size);
  MPI_Finalize();
  return 0;
}
```

</div>

<div class="col">

### Process one

```c [6,7]
int main(int argc, char **argv)
{
  int rank, size, len;
  MPI_Comm comm;
  MPI_Init(&argv, &argv);
  rank = 1;
  size = 2;
  printf("Hello, World! [%d/%d]\n",
         rank, size);
  MPI_Finalize();
  return 0;
}
```

</div>
</div>

------

## Communicators

- Messages are passed in context of a
  [_communicator_](/phys52015/notes/mpi/#communicators)
- Represent a group of processes
- Processes identified by `(communicator, rank)` pair
- Communicators available on startup
  1. `MPI_COMM_WORLD`: All processes
  2. `MPI_COMM_SELF`: Every process "on its own"
  
------

## Example with eight processes

![Illustration of communicators on eight
processes](/phys52015/images/manual/comm-world-comm-self.svg)

------

## Programming

- MPI is a _library_. No special compiler support
- Need to `#include <mpi.h>`
- Large API, we will touch on some of it

------

## Compiling

- Most MPI libraries come with _compiler wrappers_
  - `mpicc` (for C)
  - `mpicxx` (for C++)
  - `mpif90` (for Fortran)
- Take care of include and link flags

### On Hamilton

```
module load gcc/9.3.0
module load intelmpi/gcc/2019.6
```

------

## Running

- Amount of parallelism specified at _runtime_
- Use MPI library's "runner", usually `mpirun` or `mpiexec`
- On Hamilton use `mpirun`

```
$ mpirun -n 10 ./myapp
```
Run 10 copies of `myapp` in same MPI context

------

## Important concepts

- No process special
- Communicators are key
- Focus on decomposing _data_ and _work_ on that data

Next: [point to point messaging](mpi-ptp.html)
