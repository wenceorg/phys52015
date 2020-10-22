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
Fortran[^1]. These extensions come in three parts

[^1]: I won't dwell on Fortran any longer than this sentence.

1. `#pragma`-based _directives_;
2. runtime library routines;
3. environment variables for controlling runtime behaviour.

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

We can parallelise a loop like so

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

## Parallel constructs.

The basic parallel construct is a _parallel region_. This is
introduced with

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

This is a [fork/join]() or [bulk-synchronous-parallel]() programming model.
