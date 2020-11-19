---
title: "Compiler autovectorisation"
weight: 1
katex: true
---

# Obtained vectorised code

If we write code that we would like to be vectorised, we have multiple
different options available to us on how to obtain it. We'll first
list some approaches, briefly detail their strengths and weaknesses,
and then go into more detail on the approach that we'll be using in
this course.

First, some general advice. It is tempting to think of vectorisation
as an optimisation that we can apply locally to our code. In fact, we
should not think of it this way. Recalling [Amdahl's Law]({{< ref
"scaling-laws.md#amdahl" >}}), if we only speed up a small part of our
code, we might not see a particular benefit. In addition,
vectorisation might necessitate some algorithmic restructuring and
data layout transformations. These are changes which have an effect on
the performance of the whole code.

As such, I advocate designing with vectorisation (and more generally
parallelism) in mind _from the start_. Keep this in mind when we're
looking at the examples of vectorisable loops in isolation.

## Programming approaches

In order of increasing programmer control and complication, we have
broadly four options available to us to exploit vector hardware.

1. Fully automatic via the compiler

   Here we rely on automated analysis from the compiler to find
   vectorisable parts of the code and apply the appropriate
   transformation. This works well when there are simple loops.

2. Partially automatic: compiler + pragma-based source annotations

   In this case, we still want the compiler to do the work, but we
   will give it some hints by annotating our code with extra
   information. Often this is needed because we have relatively simple
   loops, but have a better idea than the compiler about whether
   vectorisation is likely to be effective, or which loops should be
   vectorised.

3. Explicit use of intrinsics

   Here we use special variable types and replace normal code by calls
   to functions that are turned into inline assembly. This is slightly
   like writing "semi-portable" assembly. This is quite a complicated
   approach, and makes your code far less readable.

   ```c
   /* Before */
   for (size_t i = 0; i < N; i++)
      c[i] = a[i] + b[i];
   /* Vectorised with AVX2 intrinsics */
   __m256d av, bv, cv;
   for (size_t i = 0; i < N; i += 4) {
       av = _mm256_load_pd(a + i);
       bv = _mm256_load_pd(b + i);
       cv = _mm256_add_pd(av, bv);
       _mm256_store_pd(c + i, cv);
   }
   ```

   {{< hint info >}}
   If you want to
   know what these functions do, see the interactive [Intel intrinsics
   guide](https://software.intel.com/sites/landingpage/IntrinsicsGuide/)
   {{< /hint >}}

   This approach is not (immediately) portable to different vector
   instruction sets (e.g. for AVX512 the load function is spelt
   `_mm512_load_pd` rather than `_mm256_load_pd`). If going down this
   route it's therefore best to wrap these things behind a library.
   For C++, [Agner Fog](https://www.agner.org) has a [nice
   one](https://github.com/vectorclass/version2).

4. Explicitly writing assembly

   This is similar to the previous case with intrinsics, but now we
   don't rely on the compiler for control flow or register allocation
   and have to do everything by hand.

There is also a further class of methods that might rely on using
domain-specific languages or language extensions. We won't touch on
them here.

Given the complexity, and lack of portability, in the third and fourth
options. In this course we will only look at vectorisation available
via compiler optimisations, sometimes helped by pragma annotations.

## Compiler vectorisation and feedback

The core idea in compiler (auto-)vectorisation is that the compiler
performs some analysis of your code, to determine whether or not
vectorisation (or in general any of the many optimisations it can
apply) is _possible_. Having determined that some strategy is
possible, the compiler then has to decide whether or not it is likely
to be profitable.

To do so, it has an (generally quite opaque) internal _cost model_.
That is, it has some model that attempts to predict how certain
optimisations will perform. For example, the cost model might say
"switching from scalar code to 4-wide AVX vectors will provide a
factor of 3x speedup". In which case, probably vectorisation would
occur.

Unfortunately, the cost models are always incomplete, and often do not
choose wisely. In these situations we can help out by annotating our
source code.

To do so, we need to know what the compiler is deciding, and why. The
incantations necessary to open this dialogue with the compiler are
version-specific. I'll focus on the Intel compiler here, but also
provide some advice for GCC.

### Optimisation reports for Intel {#opt-reports}

The Intel compiler will report information about the optimisation
decisions it made with the `-qopt-report` flag. We can control the
verbosity by saying `-qopt-report=n` for some `n=0..5`.

Let's look at how this works for a loop we hope is vectorisable

{{< code-include "optimisation-snippets/simple-vector.c" "c" >}}

Running

```sh
$ icc -qopt-report=5 -c -O3 -xCORE_AVX2 simple-vector.c
```

we get an optrpt file which we can inspect. You can also play along
with the [compiler explorer](https://gcc.godbolt.org), which is often
a more convenient way of doing things if you're just trying to check
vectorisation for a small amount of code. This example is prefilled
[here](https://gcc.godbolt.org/z/WG9xxE).

Looking at the optimisation report we see

```
...
LOOP BEGIN at <source>(5,3)
   remark #15389: vectorization support: reference x[i] has unaligned access   [ <source>(6,5) ]
   remark #15389: vectorization support: reference x[i] has unaligned access   [ <source>(6,12) ]
   remark #15389: vectorization support: reference x[i] has unaligned access   [ <source>(6,21) ]
   remark #15381: vectorization support: unaligned access used inside loop body
   remark #15305: vectorization support: vector length 4
   remark #15309: vectorization support: normalized vectorization overhead 1.429
   remark #15300: LOOP WAS VECTORIZED
   remark #15442: entire loop may be executed in remainder
   remark #15450: unmasked unaligned unit stride loads: 1
   remark #15451: unmasked unaligned unit stride stores: 1
   remark #15475: --- begin vector cost summary ---
   remark #15476: scalar cost: 10
   remark #15477: vector cost: 1.750
   remark #15478: estimated potential speedup: 5.280
   remark #15488: --- end vector cost summary ---
LOOP END
```

So the loop was vectorised, and the cost model estimates a 5.2x
speedup.

Now let's look at example of [flow dependence]({{< ref
"vectorisation#data-dep" >}}).

{{< code-include "optimisation-snippets/flow-dep.c" "c" >}}

This time, we get a report of no vectorisation

```
LOOP BEGIN at <source>(4,3)
   remark #15344: loop was not vectorized: vector dependence prevents vectorization
   remark #15346: vector dependence: assumed FLOW dependence between x[i] (5:5) and x[i-1] (5:5)
   remark #25439: unrolled with remainder by 2
   remark #25456: Number of Array Refs Scalar Replaced In Loop: 2
LOOP END
```

This says, on line 5, there is a read-after-write dependence between
the access to `x[i]` and `x[i-1]`.

These are relatively simple examples. Now let's look at one where the
compiler does the wrong thing.

{{< code-include "optimisation-snippets/gemm-microkernel.c" "c" >}}

Again, play along [on the compiler
explorer](https://gcc.godbolt.org/z/7Pf9fK).

At first glance, everything looks fine. The compiler reports
vectorisation occurred, with a modest estimated speedup.

```
...
         remark #15301: PERMUTED LOOP WAS VECTORIZED
         remark #15452: unmasked strided loads: 2 
         remark #15475: --- begin vector cost summary ---
         remark #15476: scalar cost: 8 
         remark #15477: vector cost: 5.250 
         remark #15478: estimated potential speedup: 1.470 
         remark #15488: --- end vector cost summary ---
```

However, this is a case where I, as the programmer, expect larger
gains. And the "permuted" part gives me cause for concern[^1].

[^1]: To understand why, take
    [COMP52315](https://teaching.wence.uk/comp52315/) next term!
    
Reading the rest of the report, I see

```
   remark #25444: Loopnest Interchanged: ( 1 2 3 ) --> ( 2 3 1 )
```

So the compiler obviously decided to reorder the loops and then
vectorised the (originally outermost) now innermost loop. It's cost
model is wrong!

What can we do to fix this?

### Controlling what the compiler does

In this instance, we know better than the compiler, and we want it to
vectorise the original innermost loop. To do so, we can use
[`#pragma`](https://en.wikipedia.org/wiki/Directive_(programming))
annotation to give the compiler some extra information that will help
guide its cost model (or just tell it what to do).

These pragmas are generally compiler-specific. Although for
vectorisation purposes a number are being standardised through
[OpenMP](https://www.openmp.org).

Since pragmas are compiler-specific, the canonical place to look for
information is always the compiler manual, which details supported
language extensions. Intel has [extensive
documentation](https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/pragmas/intel-specific-pragma-reference.html#intel-specific-pragma-reference),
[GCC](https://gcc.gnu.org/onlinedocs/gcc/Pragmas.html) and
[Clang](https://clang.llvm.org/docs/LanguageExtensions.html#extensions-for-selectively-disabling-optimization)
somewhat less.

A few relevant ones are

1. Loop unrolling
{{< tabs unroll >}}
{{< tab Intel >}}
```c
/* Unroll, but use heuristics */
#pragma unroll
for (i = 0; i < N; i++)
  ...
  
/* Unroll specified number of times. n must be a literal number */
#pragma unroll(n)
for (i = 0; i < N; i++)
  ...
  

/* Don't unroll */
#pragma nounroll
for ...
```

See [the
documentation](https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/pragmas/intel-specific-pragma-reference/unroll-nounroll.html#unroll-nounroll)
for details.
{{< /tab >}}

{{< tab GCC >}}
```c
/* Unroll specified number of times. n must be a literal number */
#pragma GCC unroll n
for (i = 0; i < N; i++)
  ...

/* Use n=0,1 for no unrolling */
```

See their
[docs](https://gcc.gnu.org/onlinedocs/gcc/Loop-Specific-Pragmas.html#Loop-Specific-Pragmas).
{{< /tab >}}
{{< tab clang >}}
```c
/* Unroll specified number of times. n must be a literal number */
#pragma clang loop unroll_count(n)
for (i = 0; i < N; i++)
  ...

/* Disable unrolling */
#pragma clang loop unroll(disable)
```
See their [docs](https://clang.llvm.org/docs/LanguageExtensions.html#loop-unrolling).
{{< /tab >}}
{{< /tabs >}}
2. Disregarding data dependencies
{{< tabs data-dep >}}
{{< tab Intel >}}
```c
/* Ignore assumed flow-dependence */
#pragma ivdep
for (i = k; i < N; i++)
  a[i] = a[i-k] + a[i];
```

In this case, if `k` is a runtime value, the compiler must assume that
there is a dependency (and will not vectorise). Using `#pragma ivdep`
we promise "no no, it's all fine!". See [Intel's
documentation](https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/pragmas/intel-specific-pragma-reference/ivdep.html#ivdep)
for details.

{{< hint warning >}}
You need to make sure that it actually is all fine! If `k = 1` then we
would get bad code with `ivdep`.
{{< /hint >}}

{{< /tab >}}

{{< tab GCC >}}
```c
#pragma GCC ivdep
for (i = k; i < N; i++)
  a[i] = a[i-k] + a[i];
```

See their
[docs](https://gcc.gnu.org/onlinedocs/gcc/Loop-Specific-Pragmas.html#Loop-Specific-Pragmas).
{{< /tab >}}
{{< tab clang >}}
It does not appear to be possible to specify pragmas for clang right now.
{{< /tab >}}
{{< /tabs >}}
3. Force SIMD vectorisation of loops.

   This is enabled in a cross-compiler manner using the OpenMP
   [`#pragma omp
   simd`](https://www.openmp.org/spec-html/5.0/openmpsu42.html)
   construct. You will need to enable recognition of these pragmas
   by adding `-qomp-simd` (Intel) or `-fopenmp-simd` (GCC/Clang).


Having done this, we can now go back and try annotating the code that
was treated "badly" by the compiler.

{{< code-include "optimisation-snippets/gemm-microkernel-annotated.c" "c" >}}

[This time](https://gcc.godbolt.org/z/91qc1j), we see a better result.

```
LOOP BEGIN at <source>(10,3)
   remark #15542: loop was not vectorized: inner loop was already vectorized

   LOOP BEGIN at <source>(12,5)
      remark #15542: loop was not vectorized: inner loop was already vectorized
      remark #25436: completely unrolled by 8  

      LOOP BEGIN at <source>(14,12)
         ...
         remark #15427: loop was completely unrolled
         remark #15309: vectorization support: normalized vectorization overhead 0.909
         remark #15301: OpenMP SIMD LOOP WAS VECTORIZED
```

### Was it worth it?

This micro-kernel is at the core of fast dense matrix-matrix
multiplication, and it (or assembly that is similar) is used in the
fast [BLIS](https://github.com/flame/blis/) library.

With GCC-10, rather than Intel, if I don't annotate the microkernel, I
get throughput on $ 4096 \times 4096 $ matrices of around
3.5GFlops/s on my laptop (38 seconds per multiplication). After
appropriate annotations, I get around 38GFlops/s for the same problem
(3.6 seconds per multiplication). A speedup of more than 11.

In comparison, the highly-optimised
[openblas](https://www.openblas.net) library achieves 51GFlops/s (2.7
seconds per multiplication).

### Flags for GCC and Clang

GCC and Clang also have ways of requesting information on what the
optimiser is doing. For GCC, use
[`-fopt-info`](https://gcc.gnu.org/onlinedocs/gcc/Developer-Options.html).
For Clang, use
[`-Rpass=vec`](https://clang.llvm.org/docs/UsersManual.html#rpass).
See also the [LLVM vectoriser](https://llvm.org/docs/Vectorizers.html) docs.

## Summary

Vectorisation is necessary for peak performance on modern hardware.
Generally, unless your job is developing high-performance numerical
libraries, it is best to leave the details to the compiler.

That said, we sometimes have to help the compiler's cost model along
with judicious use of pragma annotations. We saw some examples.
