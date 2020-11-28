---
title: "MPI: domain decomposition and halo exchanges"
weight: 11
katex: true
---

# Data parallel stencil computations

In this exercise we're going to explore an MPI-parallelisation of the
image blurring exercise.

The natural thing to do here is to divide the work up between
processes (much as we did for the [OpenMP version]({{< ref
openmp-stencil.md >}})).

Let's have a look at what this would look like.

{{< manfig src="image-decomposition.svg"
    width="50%"
    caption="Decomposition of a 2D image into nine pieces" >}}
    
Remember that in MPI, each process has its own memory space. So the
different processes can't directly access data that is on neighbouring
processes. So the picture is perhaps better drawn with separation
between the neighbours.

{{< manfig src="image-decomposition-split.svg"
    width="50%"
    caption="Decomposition of a 2D image showing separation of memory"
    >}}
    
This presents us with a problem, because the stencil computation needs
to read pixels in a small neighbourhood around each pixel in the
image.

We can arrange that the correct data are available by extending the
extent of the image by the stencil width (one pixel in this case),
creating _halo_ or _ghost_ regions. Before computing we copy
up-to-date data from the relevant process into the halo. We then
compute on the owned data.

{{< manfig src="image-decomposition-stencil.svg"
    width="50%"
    caption="Halo region copies onto process zero" >}}
    
The full algorithm therefore consists of a sequence of steps in which
we first perform some communication, arranging that the halo region is
up to date, followed by some computation to update the output image.

## Decomposing the image

## Setting up the halos

## Exchanging data

## Advanced: using datatypes

## Advanced: using neighbourhood collectives?
