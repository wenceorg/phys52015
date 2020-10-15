#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "vec.h"
#include "utils.h"

/* Create a vector.
 *
 * - comm: Communicator
 * - N: Global number of entries.
 * - vec: pointer to output vector structure.
 */
int VecCreate(MPI_Comm comm, int N, Vec *vec)
{
  int ierr;
  int size;
  int rank;
  Vec a = calloc(1, sizeof(struct _p_Vec));
  if (!a) {
    fprintf(stderr, "Unable to allocate space for vector\n");
    return MPI_Abort(comm, MPI_ERR_NO_MEM);
  }
  ierr = MPI_Comm_size(comm, &size);CHKERR(ierr);
  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);
  a->n = N / size;
  if (a->n * size != N) {
    fprintf(stderr, "[%d] VecCreate: need equal number of entries on each process.\n", rank);
    fprintf(stderr, "[%d] Global size %d not evenly divisible by %d.\n", rank, N, size);
    return MPI_Abort(comm, MPI_ERR_ARG);
  }
  a->N = N;
  a->comm = comm;
  a->np = size;
  a->data = calloc((size_t)a->n, sizeof(*a->data));
  if (!a->data) {
    fprintf(stderr, "Unable to allocate space for vector\n");
    return MPI_Abort(comm, MPI_ERR_NO_MEM);
  }
  *vec = a;
  return 0;
}

/* Destroy a vector.
 * - vec: pointer to vector structure (may be NULL).
 */
int VecDestroy(Vec *vec)
{
  if (!*vec) return 0;
  free((*vec)->data);
  free(*vec);
  *vec = NULL;
  return 0;
}

/* View a vector to a file.
 *
 * - x: Vector to view
 * - file: File to output to (may be NULL for standard output)
 */
int VecView(Vec x, FILE *file)
{
  int ierr;
  int rank;
  double *lvec = NULL;
  if (!file) {
    file = stdout;
  }
  ierr = MPI_Comm_rank(x->comm, &rank);CHKERR(ierr);
  if (!rank) {
    lvec = malloc(x->n * sizeof(*lvec));
    if (!lvec) {
      fprintf(stderr, "Unable to allocate space for vector\n");
      return MPI_Abort(x->comm, MPI_ERR_NO_MEM);
    }
    fprintf(file, "Vector distributed over %d processes\n", x->np);
    fprintf(file, "  Global size: %d\n", x->N);
    fprintf(file, "  Local size: %d\n", x->n);
    fprintf(file, "  Entries:\n");
    for (int i = 0; i < x->n; i++) {
      fprintf(file, "%g\n", x->data[i]);
    }
    for (int p = 1; p < x->np; p++) {
      ierr = MPI_Recv(lvec, x->n, MPI_DOUBLE, p, 1, x->comm, MPI_STATUS_IGNORE);CHKERR(ierr);
      for (int i = 0; i < x->n; i++) {
        fprintf(file, "%g\n", lvec[i]);
      }
    }
    fprintf(file, "\n");
  } else {
    ierr = MPI_Send(x->data, x->n, MPI_DOUBLE, 0, 1, x->comm);CHKERR(ierr);
  }
  return 0;
}
