---
title: "Compiler autovectorisation"
weight: 1
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
   transformation
   
2. Partially automatic: compiler + pragma-based source annotations

   In this case, we still want the compiler to do the work, but we
   will give it some hints by annotating our code with extra
   information.
   
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
