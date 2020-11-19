---
title: "Vectorisation: stencils"
weight: 3
katex: true
---

# Vectorisation of a simple stencil

We'll be running these exercises on Hamilton or COSMA, so remind
yourself of how to log in and transfer code [if you need to]({{< ref
hamilton-quickstart.md >}}).


## Blurring an image

One can blur or smooth the edges of an image by convolving the image
with a [normalised box](https://en.wikipedia.org/wiki/Box_blur)
kernel. Every output pixel \\( g_{k, l} \\) is created from the mean
of the input image pixel \\(f _{k, l}\\) and its eight neighbours.

$$
g_{k,l} = \frac{1}{9} \begin{bmatrix} 1 & 1 & 1\\\ 1 & 1 & 1\\\ 1&1&1
\end{bmatrix} * f_{k, l}
$$

This can be implemented by a loop over every pixel of the image,
accessing some a small _stencil_ of data.

This computational pattern appears in both image processing and finite
difference discretisations of partial differential equations (there is
more on the computational aspects of this in
[COMP52315](https://teaching.wence.uk/comp52315), and the numerics in
[COMP52215](https://www.dur.ac.uk/postgraduate.modules/module_description/?year=2020&module_code=COMP52215)
if you're interested).

The code for this exercise is provided as a [tar archive]({{< code-ref
blur_image.tgz >}}).

{{< hint info >}}

You can also clone the [entire course repository]({{< repo >}}), which
gives you access to the code for all the course exercises in the `code`
subdirectory.

{{< /hint >}}

Download and unpack it with

```sh
$ wget {{< code-ref blur_image.tgz >}}
$ tar zxvf blur_image.tgz
```

This will create a new directory `blur_image` with three
subdirectories.

```sh
$ cd blur_image
$ ls
images openmp vec
```

The `images` directory contains images in
[PPM](https://en.wikipedia.org/wiki/Netpbm) format for that will serve
as input to our program. For now let's look in the `vec` subdirectory.

```sh
$ cd vec
$ ls
Makefile  filters.c io.c      main.c    proto.h
```

There is some source code and a
[`Makefile`](https://www.gnu.org/software/make/) that provides a
recipe for how to build the executable. It is just a text file and can
be edited with your favourite text editor. By running `make` you build
the executable.

Before we do this, we'll have to load the right compiler modules.
We'll use the intel compiler for this exercise, since it produces
better reports than gcc.

{{< tabs compiler-modules >}}
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

If we look at the `Makefile`

{{< code-include "blur_image/vec/Makefile" "make" >}}

We can see that the compilation flags explictly turn off vectorisation
(the `-no-vec` part of `CFLAGS`).

After building the code, it produces an executable `blur`. You can run
the code with
```sh
$ ./blur input_image.ppm outout_image.ppm
```
There are some sample images in the `images` subdirectory.

{{< exercise >}}
Compile the code and run it on an input picture. Have a look at the
output.
{{< /exercise >}}

{{< question >}}
How long does the blurring take? Note that you may need to run the
same command multiple times to get a realistic timing result.

{{< details Solution >}}

For me, if I do
```
$ ./blur ../images/landscape.ppm output.ppm
Reading image file: ../images/landscape.ppm ... Done
Applying mean blur filter...
Blurring loop took:0.120000
```

So around a tenth of a second.
{{< /details >}}
{{< /question >}}

As mentioned, we explicitly turned off vectorisation. Edit the
`Makefile` to change the flags to turn on vectorisation and recompile
by running `make` again.

{{< question >}}
Did the runtime improve, or change in any other way?

{{< details Solution >}}
After removing the `-no-vec` part of the `CFLAGS` in the `Makefile`,
and recompiling (`make clean` followed by `make all`), I don't see any
change in the runtime.
{{< /details >}}
{{< /question >}}

We can ask the compiler to provide us some information on what it was
doing in the form of a _vectorisation report_.

{{< exercise >}}

Edit the compiler flags again to produce [vectorisation reports]({{<
ref "compiler.md#opt-reports" >}}) and recompile.

{{< /exercise >}}

{{< question >}}
Do the reports help to explain your observations about the runtime?

{{< details Solution >}}

After adding `-qopt-report=5` to the `CFLAGS` and recompiling, I see,
in `filters.optrpt` the following:

```
LOOP BEGIN at filters.c(38,3)
   remark #15344: loop was not vectorized: vector dependence prevents vectorization
   remark #15346: vector dependence: assumed OUTPUT dependence between output->r[i] (39:5) and output (41:5)
   remark #15346: vector dependence: assumed OUTPUT dependence between output (41:5) and output->r[i] (39:5)
   remark #25439: unrolled with remainder by 2
LOOP END
```

For the initial zeroing loop, and 

```
LOOP BEGIN at filters.c(48,3)
   remark #25096: Loop Interchange not done due to: Imperfect Loop Nest (Either at Source or due to other Compiler Transformations)
   remark #25451: Advice: Loop Interchange, if possible, might help loopnest. Suggested Permutation : ( 1 2 3 ) --> ( 2 3 1 ) 
   remark #15541: outer loop was not auto-vectorized: consider using SIMD directive

   LOOP BEGIN at filters.c(53,5)
      remark #25096: Loop Interchange not done due to: Imperfect Loop Nest (Either at Source or due to other Compiler Transformations)
      remark #25452: Original Order found to be proper, but by a close margin
      remark #15541: outer loop was not auto-vectorized: consider using SIMD directive

      LOOP BEGIN at filters.c(60,7)
         remark #15344: loop was not vectorized: vector dependence prevents vectorization
         remark #15346: vector dependence: assumed OUTPUT dependence between output->r[id] (70:9) and output (72:9)
         remark #15346: vector dependence: assumed OUTPUT dependence between output (72:9) and output->r[id] (70:9)
      LOOP END
   LOOP END
LOOP END
```

For the main blurring loop.
{{< /details >}}
{{< /question >}}

The main computational part of the code is in `filters.c`. Have a look
at that file.

{{< question >}}
Was the compiler able to vectorise the loops? If not, explain why.

{{< details Solution >}}

From the optimisation reports, we see that the compiler can't
vectorise the loops. It complains about an `OUTPUT` dependence when
writing to `output->r[i]`.

The reason for this is that in C, arrays are allowed to
[alias](https://en.wikipedia.org/wiki/Pointer_aliasing). As a result,
the compiler cannot prove that the writes to `output->r[i]` and
`output->g[i]` are independent: it thinks there is a data race.

We know this is safe, however, so if we annotate the loops with
`#pragma omp simd` and add `-qopenmp-simd` to the CFLAGS, then the
compiler will vectorise the loops. If I also add `-xCORE_AVX2` then
when I run with a blur filter width of 10 (`n = 10` in `main.c`) then
before I get:

```
$ ./blur ../images/landscape.ppm output.ppm
Reading image file: ../images/landscape.ppm ... Done
Applying mean blur filter...
Blurring loop took:2.970000
```

As opposed to after

```
$ ./blur ../images/landscape.ppm output.ppm
Reading image file: ../images/landscape.ppm ... Done
Applying mean blur filter...
Blurring loop took:1.300000
```

{{< /details >}}
{{< /question >}}

{{< exercise >}}

Produce a plot of the runtime as a function of the size of the blur
stencil (`n` in the `main` function).

Have a look at the code in `filter.c`. How do you think the runtime
should behave as a function of `n`. Does this match with your
observations?

{{< details Solution >}}

For each pixel in the output image, there is a double loop from $[-n,
n]$, accessing $(2n+1)^2 + 1$ pixels in the input image, so I would
expect that the time to blur the image is quadratic in the size of
`n`. Let's check.

I edit the `main.c` to add a loop over `n` around the `blur_mean`
call. I don't care about the output, so I remove that, however, I need
to remember to free the output image each time (otherwise I leak memory).

```c
for (int n = 1; n < 17; n++) {
  blur_mean(myimage, n, &output);
  free_image(&output);
}
```

Then I run with

```
$ ./blur ../images/landscape.ppm output.ppm | grep Blurring | cut -f 2 -d :
0.050000
0.100000
0.190000
0.280000
0.310000
0.460000
0.600000
0.790000
0.960000
1.210000
1.450000
1.770000
2.130000
2.530000
3.100000
3.470000
```

Let's plot these

```python
import numpy
from matplotlib import pyplot
data = numpy.asarray([0.05, 0.1, 0.19, 0.28, 0.31, 0.46, 0.6,
                      0.79, 0.96, 1.21, 1.45, 1.77, 2.13,
                      2.53, 3.1, 3.47])
ns = numpy.arange(1, 17)
fig, axes = pyplot.subplots(1)
axes.xlabel("n")
axes.ylabel("Time [s]")
axes.plot(ns, data, "o", label="Timing data")
# Fit a quadratic polynomial to the data
a, b, c = numpy.polyfit(ns, data, 2)
axes.plot(ns, a*ns**2 + b*ns + c, "-", label="Quadratic best fit")
axes.legend(loc="best")
```

{{< autofig
    src="vectorised-blur-image-algorithmic-scaling.svg"
    width="100%"
    caption="The algorithmic scaling of the blur filter is quadratic in `n`" >}}

{{< /details >}}
{{< /exercise >}}
