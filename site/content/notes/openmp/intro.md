---
title: "What is OpenMP?"
weight: 1
katex: false
---

# What is OpenMP

[OpenMP](https://www.openmp.org) is a
[standardised](https://www.openmp.org/wp-content/uploads/OpenMP-API-Specification-5.0.pdf)
API for programming shared memory computers (and more recently
[GPUs](https://en.wikipedia.org/wiki/General-purpose_computing_on_graphics_processing_units))
using threading as the programming paradigm. It supports both
data-parallel shared memory programming (typically for parallelising
loops) and task parallelism. We'll see some examples later.

In recent years, it has also gained support for some vector-based
parallelism.

## Using OpenMP

OpenMP is implemented as a set of extensions for C, C++, and
Fortran. These extensions come in three parts

1. `#pragma`-based _directives_;
2. runtime library routines;
3. environment variables for controlling runtime behaviour.

OpenMP is an _explicit_ model of parallel programming. It is your job,
as the programmer, to decide where and how to employ parallelism.

### Directives

We already saw some directives when discussing vectorisation. In that
case, we saw compiler-specific directives. In the case of OpenMP,
since it is a standard, the meaning of the directive is the same
independent of the compiler choice[^2].

[^2]: Modulo bugs in the compiler's implementation.

All OpenMP directives start with `#pragma omp `. They are therefore
ignored if the code is compiled without adding some special compiler
flags.

{{< tabs "Compiler flags" >}}
{{< tab Intel >}}
`-qopenmp`
{{< /tab >}}
{{< tab "GCC/Clang" >}}
`-fopenmp`
{{< /tab >}}
{{< /tabs >}}

We can [parallelise a loop]({{< ref "loop-parallelism.md" >}}) like so

```c
#pragma omp parallel for
for (int i = 0; i < N; i++)
  ...
```

### Library routines

In addition to the directives, which are used to enable parallelism
through annotation, the OpenMP standard also provides for a number of
runtime API calls. These allow threads to inspect the state of the
program (for example to ask which thread they are). To do this, we
must include a C header file `omp.h`. All OpenMP API calls are
prefixed with `omp_`.

```c
#include <omp.h>

...
#pragma omp parallel
{
  /* Which thread am I? */
  int threadid = omp_get_thread_num();
  /* How many threads are currently executing */
  int nthread = omp_get_num_threads();
}
```

Unlike the pragmas, these runtime calls are only available when
compiling with OpenMP enabled[^3].

[^3]: The Intel compiler supports a "stub" library that you can use
    with `-qopemp-stubs` if you want to compile in serial.

### Environment variables

The primary variable is `OMP_NUM_THREADS` which specifies the number of
threads that should be available to the program when running.

This is the number of threads created when entering a
parallel region.

{{< code-include "hello/openmp.c" "c" >}}

```sh
$ icc -qopenmp -o hello openmp.c
$ export OMP_NUM_THREADS=1
$ ./hello
Hello, World! I am thread 0 of 1
$ export OMP_NUM_THREADS=8
$ ./hello
Hello, World! I am thread 5 of 8
Hello, World! I am thread 3 of 8
Hello, World! I am thread 6 of 8
Hello, World! I am thread 1 of 8
Hello, World! I am thread 0 of 8
Hello, World! I am thread 2 of 8
Hello, World! I am thread 4 of 8
Hello, World! I am thread 7 of 8
```

{{< exercise >}}
Try this yourself. For instructions you can also see the [Hello
World]({{< ref "hello.md" >}}) exercise.

What do you observe if you never set `OMP_NUM_THREADS`?

{{< details "Hint" >}}
Use `unset OMP_NUM_THREADS` to ensure there is no existing value of
the variable.
{{< /details >}}
{{< /exercise >}}

{{< hint warning >}}
The number of threads used by an OpenMP program if you do not set
`OMP_NUM_THREADS` explicitly is up to the particular implementation.

I therefore recommend that you _always_ set `OMP_NUM_THREADS`
explicitly.
{{< /hint >}}

## Parallel constructs.

The basic parallel construct is a _parallel region_. This is
introduced with [`#pragma omp parallel`](https://computing.llnl.gov/tutorials/openMP/#ParallelRegion)

```c
/* Some serial code on thread0 */
#pragma omp parallel /* Extra threads created */
{
  /* This code is executed in parallel by all threads. */
  ...;
} /* Synchronisation waiting for all threads */
/* More serial code on thread0 */
```

The program begins by executing using a single thread (commonly termed
the master thread, though we'll use the phrase "thread0"). When a
parallel region is encounter, a number of additional threads (called a
"team") are created. These threads all execute the code inside the
parallel region, there is then a synchronisation point at the end of
the region, after which thread0 continues execution of the next
(serial) statements.

This is a [fork/join](https://en.wikipedia.org/wiki/Fork–join_model)
programming model, and is therefore best analysed using the [bulk
synchronous parallel
(BSP)](https://en.wikipedia.org/wiki/Bulk_synchronous_parallel)
abstract model.

{{< manfig
    src="fork-join-schematic.svg"
    width="75%"
    caption="Schematic of fork-join parallelism. Single-threaded execution above (with two regions of parallel tasks). Parallel execution with fork-join points marked below." >}}

```c
void foo(...)
{
  ...;
  #pragma omp parallel
  {
    parallel_code;
  } /* Synchronisation here */
  serial_code;
  #pragma omp parallel
  {
    more_parallel_code;
  } /* Synchronisation here */
}
```

## Data scoping: shared and private

Inside a parallel region, any variables that are in scope can either
be _shared_ or _private_. All threads see the same (single) copy of
any shared variables and can read and write to them. Private variables
are individual to each thread: there is one copy of the variable per
thread. These variables are not visible to any other threads in the
region, and can thus only be read and written by their own thread.

We declare the visibility of variables by providing extra clauses to
the `parallel` directive

{{< code-include "openmp-snippets/parallel-region.c" "c" >}}

{{< exercise >}}
Compile and run this code with a number of different threads.

Convince yourself you understand what is going on.
{{< /exercise >}}

### Default data scoping

If we have a lot of variables that we want to access in the parallel
region, it is slightly tedious to explicitly list them. 

Consider, for example, the following snippet
```c
double *a;
int N;
int i, j;
#pragma omp parallel shared(a) private(i)
{
  /* a is shared, i is private (explicitly) */
  /* N and j are also in scope, so they must be either shared or private. */
}
```

The OpenMP design board took the decision that there should be a
default scoping rule for variables.

This was a **terrible** decision.

Neither choice is particularly satisfying, but the default, in the
absence of specification, is that variables are _shared_.

This is bad, because if we write seemingly innocuous code, it can
behave badly when run in parallel.

Try compiling and running this code, very similar to the
parallel-region example we saw above

{{< code-include "openmp-snippets/parallel-region-bad.c" "c" >}}

{{< exercise >}}

Compile and run the code using 8 threads a number of times. What do
you observe about the output?

Can you explain what is happening?

{{< details Hint >}}
Think about the potential [data races]({{< ref "openmp#sync-data-race" >}}).
{{< /details >}}

{{< details Solution >}}
If I run this code on eight processes, I see:

```
$ OMP_NUM_THREADS=8 ./bad-region
a[ 0] = 7
a[ 1] = 1
a[ 2] = 7
a[ 3] = 3
a[ 4] = 7
a[ 5] = 5
a[ 6] = 6
a[ 7] = 7
```

Although sometimes the values change.

What is happening is that the `i` variable which records the thread
number in the parallel region is _shared_ (rather than being private).
So by the time we get to the point where we write it into the output
array, it is probably overwritten by a value from another thread.

{{< /details >}}
{{< /exercise >}}


Fortunately, there is a way to negate this bad design decision. The
`default` clause. If we write

```c
#pragma omp parallel default(none)
{
}
```

Then we must explictly provide the scoping rules for all variables we
use in the parallel region. This forces us to think about what the
right scope should be.


{{< hint warning >}}

My recommendation is to **always** use `default(none)` in parallel
directives. It might start out tedious, but it will save you many
subtle bugs!

{{< /hint >}}

### Values on entry

Shared variables take inherit the value outside the parallel region
inside. Private variables are uninitialised.

{{< code-include "openmp-snippets/uninitialised.c" "c" >}}

At least some compilers will warn about this.
```
$ gcc-10 -Wall -Wextra -o uninitialised uninitialised.c -fopenmp
uninitialised.c: In function 'main._omp_fn.0':
uninitialised.c:13:5: warning: 'b' is used uninitialized in this function [-Wuninitialize]
   13 |     printf("%d %d\n", omp_get_thread_num(), b);
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uninitialised.c:6:7: note: 'b' was declared here
    6 |   int b = 42;
      |       ^
```

Running the code produces some (possibly surprising) results.

```
$ OMP_NUM_THREADS=8 ./uninitialised
4 32642
1 32642
3 32642
2 32642
0 0
6 32642
7 32642
5 32642
```

{{< exercise >}}
If you do this, do you always see the same nonsense values? Does it
depend on the compiler?

{{< details Solution >}}
I, at least, don't always see the same values. Although it seems for
me, thread0 always gets initialised to zero.
{{< /details >}}
{{< /exercise >}}

If you _really_ need a private variable that takes its initial value
from the surrounding scope you can use the `firstprivate` clause. But
it is rare that this is necessary.

```c
int b = 23;
double *a = malloc(4*sizeof(*a));
#pragma omp parallel default(none) firstprivate(b) shared(a)
{
  int i = omp_get_thread_num();
  if (i < 4) {
    a[i] = b + i;
  }
}
```

## Summary

OpenMP provides a directives + runtime library approach to shared
memory parallelism using threads. It uses the fork/join model of
parallel execution. Threads can diverge in control-flow, and can
either share variables, or have their own copies.

With the ability to create threads and have a unique identifier for
each thread, it is possible to program a lot of parallel patterns "by
hand". For example, dividing loop iterations up between threads.

This is rather hard work, and OpenMP provides a number of additional
parallelisation directives which we will look at next that help with
[parallelisation of loops]({{< ref "loop-parallelism.md" >}})
