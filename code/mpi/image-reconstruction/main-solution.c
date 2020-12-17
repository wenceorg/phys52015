#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include "image.h"
#include "pgmio.h"

static inline int linear_index(int i, int j, int NX)
{
  return i*NX + j;
}

/* Detect edges in input image using a Laplacian filter and produce
   output "edges" image. Allocates output image. */
void EdgeDetect(Image input, Image *edges)
{
  int NX = input->NX;
  int NY = input->NY;
  CreateImage(edges);
  SetSizes(*edges, NX, NY);
  SetThreshold(*edges, input->threshold);

  float left_right_boundary_val = 0;
  for (int i = 0; i < NY; i++) { /* rows */
    for (int j = 0; j < NX; j++) { /* columns */
      int ij = linear_index(i, j, NX);
      int ijm1 = linear_index(i, j-1, NX);
      int ijp1 = linear_index(i, j+1, NX);
      int im1j = linear_index(i-1, j, NX);
      int ip1j = linear_index(i+1, j, NX);
      float vij, vijm1, vijp1, vim1j, vip1j;

      vij = input->data[ij];
      vim1j = (i == 0) ? input->top_boundary[j] : input->data[im1j];
      vip1j = (i == NY-1) ? input->bottom_boundary[j] : input->data[ip1j];
      vijm1 = (j == 0) ? left_right_boundary_val : input->data[ijm1];
      vijp1 = (j == NX-1) ? left_right_boundary_val : input->data[ijp1];
      /*
       * out[i, j] = in[i-1, j] + in[i+1, j] + in[i, j-1] + in[i, j+1] - 4*in[i, j];
       * For out of bounds accesses (edge of image), use boundary_val.
       */
      (*edges)->data[ij] = (vijm1 + vijp1 + vim1j + vip1j - 4*vij);
    }
  }
}

void DistributeImage(Image input, Image *output, int root, MPI_Comm comm)
{
  /* Distribute rows of input image (defined on root rank) to output
     image. */
  /* Use MPI_Bcast to send the image size and threshold */
  /* You can use MPI_Scatter (if the number of ranks evenly divides
     the number of rows) or MPI_Scatterv otherwise to send the data */

  int rank, size;
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  /* The input image is only available on the root rank,
   * so get information about it and broadcast to everyone. */
  int NXYt[3];
  if (rank == root) {
    NXYt[0] = input->NX;
    NXYt[1] = input->NY;
    NXYt[2] = input->threshold;
  }
  MPI_Bcast(NXYt, 3, MPI_INT, root, comm);

  int NX = NXYt[0];
  int NY = NXYt[1];
  int threshold = NXYt[2];

  /* split up rows */
  int nrows = NY / size + ((NY % size) > rank);

  /* Everyone is going to have an output image of size NX*nrows */
  CreateImage(output);
  SetSizes(*output, NX, nrows);
  SetThreshold(*output, threshold);

  if ((NY / size) * size == NY) {
    /* Everyone has the same number of rows, so can MPI_Scatter. */
    /* Can use MPI_Scatter */
    MPI_Scatter(input ? input->data : NULL,
                NX * nrows, MPI_FLOAT,
                (*output)->data, NX * nrows, MPI_FLOAT,
                0, comm);
  } else {
    /* We have different numbers of rows on the different processes,
     * figure out how many and use MPI_Scatterv. */
    int *sendcounts = NULL;
    int *displs = NULL;
    if (rank == root) {
      int displ = 0;
      sendcounts = malloc(size * sizeof(*sendcounts));
      displs = malloc(size * sizeof(*sendcounts));
      for (int i = 0; i < size; i++) {
        /* How many items to send to rank i */
        sendcounts[i] = (NY / size + ((NY % size) > i)) * NX;
        /* Offset in the input image where we start sending from to
           rank i. */
        displs[i] = displ;
        displ += sendcounts[i];
      }
    }
    /* Locally, I just receive NX * nrows contiguous float values
       into the output image. */
    MPI_Scatterv(input ? input->data : NULL,
                 sendcounts, displs, MPI_FLOAT,
                 (*output)->data, NX * nrows, MPI_FLOAT,
                 0, comm);
    free(sendcounts);
    free(displs);
  }
}


void GatherImage(Image input, Image *output, int root, MPI_Comm comm)
{
  /* Gather rows of the input image (defined on all ranks) to output
     image (defined on root rank). */
  int size, rank;
  int NY = 0;
  int threshold = 0;
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);
  /* Figure out global image size */
  MPI_Allreduce(&input->NY, &NY, 1, MPI_INT, MPI_SUM, comm);
  /* Get threshold */
  MPI_Reduce(&input->threshold, &threshold, 1, MPI_INT, MPI_MAX, root, comm);

  if (rank == root) {
    /* root rank makes an image with the newly figured out global image size */
    CreateImage(output);
    SetSizes(*output, input->NX, NY);
    SetThreshold(*output, threshold);
  } else {
    *output = NULL;
  }

  if ((NY / size) * size == NY) {
    /* All subimages the same size, can use MPI_Gather */
    MPI_Gather(input->data, input->NX * input->NY, MPI_FLOAT,
               *output ? (*output)->data : NULL, input->NX * input->NY,
               MPI_FLOAT,
               root, comm);
  } else {
    /* Subimages are different sizes, so we need to use Gatherv. */
    int *recvcounts = NULL;
    int *displs = NULL;
    if (rank == root) {
      int displ = 0;
      recvcounts = malloc(size * sizeof(*recvcounts));
      displs = malloc(size * sizeof(*recvcounts));
      /* Figure out how much to receive from each rank and where in
         the output buffer to put them. */
      for (int i = 0; i < size; i++) {
        recvcounts[i] = (NY / size + ((NY % size) > i)) * input->NX;
        displs[i] = displ;
        displ += recvcounts[i];
      }
    }
    /* And gather everything to root rank. */
    MPI_Gatherv(input->data, input->NX*input->NY, MPI_FLOAT,
                *output ? (*output)->data : NULL, recvcounts, displs,
                MPI_FLOAT, root, comm);
    free(recvcounts);
    free(displs);
  }
}

void ReconstructFromEdges(Image edges, int niterations, MPI_Comm comm, Image *output)
{
  Image old = NULL;
  Image new = NULL;
  int NX = edges->NX;
  int NY = edges->NY;
  int rank, size;
  MPI_Request requests[4];

  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  CreateImage(&new);
  SetSizes(new, NX, NY);
  SetThreshold(new, edges->threshold);

  /* Copy edges into old image */
  CopyImage(edges, &old);

  float left_right_boundary_val = 0;
  /* Run niterations Jacobi iterations to invert the Laplacian,
   * reconstructing an output image from its edges. */

  /* Who are we receiving from (and/or) sending too? MPI_PROC_NULL can
   * be used as an "empty" destination. Messages to and/or from there
   * will always return immediately and do nothing. Simplifies code a
   * bit.
   *
   *   ---------------
   *  |               |
   *  |               |
   *  |               |
   *  |               |
   *  |     ABOVE     |<-\
   *   ---------------   | Send top row to above rank
   *  |               |--/
   *  |               |
   *  |               |
   *  |               |
   *  |               |--\
   *   ---------------   | Send bottom row to below rank
   *  |     BELOW     |<-/
   *  |               |
   *  |               |
   *  |               |
   *  |               |
   *   ---------------
   */
  int above = rank == 0 ? MPI_PROC_NULL : rank - 1;
  int below = rank == size - 1 ? MPI_PROC_NULL : rank + 1;

  for (int it = 0; it < niterations; it++) {
    /* Insert boundary values from my neighbours here.
     *
     * the top_boundary comes from the last row of my neighbour with a
     * lower rank
     *
     * the bottom_boundary comes from the first row of my neighbour
     * with a higher rank
     *
     * If I'm at the very top of the image, then I can leave the
     * top_boundary alone
     *
     * If I'm at the very bottom of the image, then I can leave the
     * bottom_boundary alone.
     */

    MPI_Irecv(old->top_boundary, NX, MPI_FLOAT, above, 0, comm, &requests[0]);
    MPI_Irecv(old->bottom_boundary, NX, MPI_FLOAT, below, 0, comm, &requests[1]);
    MPI_Issend(old->data, NX, MPI_FLOAT, above, 0, comm, &requests[2]);
    MPI_Issend(&old->data[linear_index(0, old->NY - 1, old->NX)], NX, MPI_FLOAT, below, 0, comm, &requests[3]);
    MPI_Waitall(4, requests, MPI_STATUSES_IGNORE);

    /* Do local iteration */
    for (int i = 0; i < NY; i++) { /* rows */
      for (int j = 0; j < NX; j++) { /* columns */
        int ij = linear_index(i, j, NX);
        int ijm1 = linear_index(i, j-1, NX);
        int ijp1 = linear_index(i, j+1, NX);
        int im1j = linear_index(i-1, j, NX);
        int ip1j = linear_index(i+1, j, NX);

        float vij, vijm1, vijp1, vim1j, vip1j;

        vij = edges->data[ij];
        vim1j = (i == 0) ? old->top_boundary[j] : old->data[im1j];
        vip1j = (i == NY-1) ? old->bottom_boundary[j] : old->data[ip1j];
        vijm1 = (j == 0) ? left_right_boundary_val : old->data[ijm1];
        vijp1 = (j == NX-1) ? left_right_boundary_val : old->data[ijp1];
        new->data[ij] = 0.25*(vijm1 + vijp1 + vim1j + vip1j) - 0.25*vij;
      }
    }
    Image tmp;
    tmp = old;
    old = new;
    new = tmp;
  }
  *output = old;
  DestroyImage(&new);
}

int main(int argc, char **argv)
{
  Image edges = NULL, distributed_reconstructed = NULL;
  Image reconstructed = NULL, distributed_edges = NULL;
  int niterations = 10;
  MPI_Comm comm;
  int rank, size;

  MPI_Init(&argc, &argv);

  comm = MPI_COMM_WORLD;
  if (argc != 5) {
    fprintf(stderr, "Usage: %s INPUT EDGES RECONSTRUCTED NITERATIONS\n", argv[0]);
    MPI_Finalize();
    return 1;
  }

  niterations = atoi(argv[4]);

  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  if (rank == 0) {
    Image input = NULL;
    ReadImage(argv[1], &input);
    EdgeDetect(input, &edges);
    DestroyImage(&input);
    WriteImage(argv[2], edges);
  }

  /* Scatter image to processes, dividing it up */
  DistributeImage(edges, &distributed_edges, 0, comm);

  DestroyImage(&edges);

  {
    /* Write the distributed edge images. */
    char filename[100];
    size_t ret;
    ret = snprintf(filename, 100, "rank%d-%s", rank, argv[2]);
    if (ret >= 100) {
      fprintf(stderr, "Filenames too long for edge file debug output\n");
    } else {
      WriteImage(filename, distributed_edges);
    }
  }

  /* Run local reconstruction (will need modifying) */
  ReconstructFromEdges(distributed_edges, niterations, comm,
                       &distributed_reconstructed);

  DestroyImage(&distributed_edges);
  {
    /* Write the distributed reconstructed images. */
    char filename[100];
    size_t ret;
    ret = snprintf(filename, 100, "rank%d-%s", rank, argv[3]);
    if (ret >= 100) {
      fprintf(stderr, "Filenames too long for reconstructed file debug output\n");
    } else {
      WriteImage(filename, distributed_reconstructed);
    }
  }
  /* Gather image to rank 0 for writing */
  GatherImage(distributed_reconstructed, &reconstructed, 0, comm);

  DestroyImage(&distributed_reconstructed);

  if (rank == 0) {
    WriteImage(argv[3], reconstructed);
  }

  DestroyImage(&reconstructed);

  MPI_Finalize();
  return 0;
}
