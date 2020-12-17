---
title: "MPI: domain decomposition and halo exchanges"
weight: 11
katex: true
---

# Data parallel stencil computations

In this exercise we're going to explore an MPI-parallelisation of a
simple image processing problem.

In particular, we are going to reconstruct an image from its edges.

## Introduction and background

A particularly simple way of detecting the edges in an image is to
convolve it with a [Laplacian
kernel](https://aishack.in/tutorials/sobel-laplacian-edge-detectors/).
That is, given some image $I$, we can obtain its edges with

$$
E = \nabla^2 I.
$$

I discretise the $\nabla^2$ operator with a [five-point finite
difference stencil](https://en.wikipedia.org/wiki/Five-point_stencil).

Given an image of edges $E$, we can (approximately) reconstruct the
image $I$ by applying the inverse $\left(\nabla^2\right)^{-1}$. The
approximate reconstructed image is

$$
I^r = \left(\nabla^2\right)^{-1} E.
$$

We discretise the image as a 2D array, computing the edges can be done
in one step

$$
E_{i, j} = I_{i-1, j} + I_{i+1, j} + I_{i, j-1} + I_{i, j+1} - 4I_{i,j}
$$

for all pixels $i, j$ in the image.

To reconstruct the image, we will use a [Jacobi
iteration](https://en.wikipedia.org/wiki/Jacobi_method), given an
initial guess for the reconstructed image, $I^{r, 0}$, we set
$k = 0$ and then an improved guess is given by

$$
I_{i,j}^{r,k+1} = \frac{1}{4}\left(I_{i-1, j}^{r,k} + I_{i+1,j}^{r,k} +
I_{i,j-1}^{r,k} + I_{i,j+1}^{r,k} - E_{i,j}\right)
$$

for all pixels $i, j$ in the image.

After many many iterations ($k \to \infty$), this will converge to a
good approximation to the initial image.

{{< hint danger >}}

**WARNING, WARNING**. This is a _terrible_ way of inverting the
Laplacian, we're using it to illustrate some domain decomposition and
parallelisation approaches.

If you actually want to do this kind of thing in the wild, please get
in touch and we can figure out a better way.

{{< /hint >}}


## Implementation

In the `code/mpi/image-reconstruction` subdirectory, I provide some
skeleton code. It runs correctly in serial to reconstruct images.
Build the code with `make` and then run it like so (there are some
sample images in the `images` subdirectory)

```
$ ./main images/mario.pgm edges.pgm reconstructed.pgm 100
```

The detected edges are written to `edges.pgm`, the reconstructed image
to `reconstructed.pgm` and the number of iterations is selected as
100.

{{< exercise >}}

Build and run the code and look at the input, edge, and reconstructed
images.

With X-forwarding enabled (`ssh -Y` or `ssh -X`) you can view these
images on Hamilton with `display imagename`.

{{< /exercise >}}

### Parallelisation

To parallelise this code we need to do three things:

1. Split the input image up across the processes;
2. Correctly exchange data during the Jacobi iteration;
3. Gather the partial images for final output.

I've already implemented gathering (in `GatherImage`), you need to
implement `DistributeImage` (to split the image up) and add
communication in `ReconstructFromEdges`.

### Data distribution

The parallelisation strategy we use is termed domain decomposition. We
divide up the whole domain (in this case the image) approximately
equally among all processes. For 2D problems, we would usually use a
two-dimensional decomposition, as illustrated in the figure below.

{{< manfig src="image-decomposition.svg"
    width="50%"
    caption="Decomposition of a 2D image into nine pieces" >}}

To simplify the implementation here, we'll only use a one-dimensional
distribution. Each process will work on a contiguous chunk of image
rows.

An image object is represented as a pointer to a struct

```c
struct _p_Image {
  int NX;
  int NY;
  int threshold;
  float *data;
  float *top_boundary;
  float *bottom_boundary;
};
typedef struct _p_Image * Image;
```

For an image, the number of rows is `image->NY`, the number of columns
is `image->NX`. The threshold (the maximum value in the image) is in
`image->threshold` (you should just copy this around).

The image data itself is in `image->data`, to index, I provide a
`linear_index` function:

```c
int linear_index(int row, int col, int NX);
```

which turns a row and and column index (counting from the top-left
corner) into a single index into `image->data`. An image distribution
on two processes is shown below.

{{< manfig src="image-two-ranks.svg"
    width="40%"
    caption="Decomposition of a 2D image between two processes. " >}}

There are some utility functions for allocating images and setting up
the sizes. To create an image with space allocated we first create an
empty image and then set the number of pixels in the X and Y
directions (these should be the number of pixels for the local part of
the image), along with the threshold (you can either set this to 255,
or else copy it from an existing image):

```c
Image image;
CreateImage(&image);
SetSizes(image, NX, NY);
SetThreshold(image, threshold);

/* Deallocate all data */
DestroyImage(&image);
```

{{< exercise >}}

Implement distribution of the image amongst the processes. You can
look at the `GatherImage` function for inspiration. You should scatter
the input image (read on rank 0) across all processes using the
inverse of the gathering approach.

Having done this, you can now run the code in parallel. It will work,
but will produce incorrect answers since at the boundary of the
partial images there is no data exchange.

{{< details Hint >}}

I save the partial images on each process with names starting with
`rankXX`, looking at these might help to debug if things go wrong.

{{< /details >}}

{{< /exercise >}}


### Halo exchange

Remember that in MPI, each process has its own memory space. So the
different processes can't directly access data that is on neighbouring
processes. So the picture is perhaps better drawn with separation
between the neighbours.

{{< manfig src="image-two-ranks-split.svg"
    width="30%"
    caption="Decomposition of a 2D image showing separation of memory"
    >}}
    
This presents us with a problem, because the stencil computation needs
to read pixels in a small neighbourhood around each pixel in the
image.

We can arrange that the correct data are available by extending the
extent of the image by the stencil width (one pixel in this case),
creating _halo_ or _ghost_ regions. Before computing we copy
up-to-date data from the relevant process into the halo. We then
compute on the owned data. These are the `top_boundary` and
`bottom_boundary` arrays in the image struct. This copying is shown
schematically below.

{{< manfig src="image-decomposition-split-halo.svg"
    width="40%"
    caption="Halo region copies between processes" >}}

The image struct contains some space for these boundaries in the
`top_boundary` and `bottom_boundary` arrays. The code in
`ReconstructFromEdges` is already setup to use these correctly, you
just need to populate them with the correct values.

{{< hint info >}}

Usually we would allocate halo data as part of the full image array
and adjust our loops accordingly. The approach I use here simplifies
the implementation a little bit, and is conceptually equivalent.

{{< /hint >}}

{{< exercise >}}

You should implement the halo exchange in the `ReconstructFromEdges`
function. There's a comment indicating where it should occur.

With this done, you should be able to run the code in parallel and
recover images from their edges.

{{< details Hint >}}

You should need two receives (one from above one from below) and two
sends (similarly). To avoid the need to special case at the edges of
the full image, you can use `MPI_PROC_NULL` as a source or destination
rank. This turns any message into a "no-op" which returns immediately.

{{< /details >}}

{{< details Solution >}}

The directory contains an annotated solution file as
[`mpi/image-reconstruction/main-solution.c`]({{< code-ref
"mpi/image-reconstruction/main-solution.c" >}})
{{< /details >}}

{{< /exercise >}}

