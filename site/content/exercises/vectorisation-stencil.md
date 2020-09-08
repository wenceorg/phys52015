---
title: "Vectorisation: stencils"
weight: 3
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

{{< task >}}
Compile the code and run it on an input picture. Have a look at the
output.
{{< /task >}}

{{< question >}}
How long does the blurring take? Note that you may need to run the
same command multiple times to get a realistic timing result.
{{< /question >}}

As mentioned, we explicitly turned off vectorisation. Edit the
`Makefile` to change the flags to turn on vectorisation and recompile
by running `make` again.

{{< question >}}
Did the runtime improve, or change in any other way?
{{< /question >}}

We can ask the compiler to provide us some information on what it was
doing in the form of a _vectorisation report_.

{{< task >}}
Edit the compiler flags again to produce vectorisation reports and
recompile.
{{< /task >}}

{{< question >}}
Do the reports help to explain your observations about the runtime?
{{< /question >}}

The main computational part of the code is in `filters.c`. Have a look
at that file.

{{< question >}}
Why do you think the compiler is unable to vectorise the loops?
{{< /question >}}

{{< question >}}
Can you think of a way of rewriting the code (perhaps by padding
arrays) to obtain a vectorisable loop?
{{< /question >}}
