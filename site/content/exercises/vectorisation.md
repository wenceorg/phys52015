---
title: "Vectorisation"
weight: 2
---

# Vectorisation

We'll be running these exercises on Hamilton or COSMA, so remind
yourself of how to log in and transfer code [if you need to]({{< ref
hamilton-quickstart.md >}}).

## Blurring an image

The code for this exercise is provided as a [tar archive]({{< code-ref
blur_image.tgz >}}). Download and unpack it with

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

