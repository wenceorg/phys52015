#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

static void print_matrix(const double *A, int n, MPI_Comm comm)
{
  /* Implement printing of A by gathering to rank 0 and printing there. */

  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);
  if (rank == 0) {
    double *Afull = malloc(n*size*sizeof(*Afull));
    MPI_Gather(A, n, MPI_DOUBLE, Afull, n, MPI_DOUBLE, 0, comm);
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        printf("%g ", Afull[i*n + j]);
      }
      printf("\n");
    }
    printf("\n");
    free(Afull);
  } else {
    MPI_Gather(A, n, MPI_DOUBLE, NULL, 0, MPI_DATATYPE_NULL, 0, comm);
  }
}

static void transpose_matrix(const double *A, double *AT, int n, MPI_Comm comm)
{
  /* Implement AT <- transpose(A) (MPI_Alltoall may be helpful) */
  MPI_Alltoall(A, 1, MPI_DOUBLE, AT, 1, MPI_DOUBLE, comm);
}

static void matrix_vector_product(const double *A, const double *x, double *y, int n, MPI_Comm comm)
{
  int rank;
  double *AT = malloc(n*sizeof(*AT));
  double *ypartial = malloc(n*sizeof(*ypartial));
  transpose_matrix(A, AT, n, comm);
  for (int i = 0; i < n; i++) {
    ypartial[i] = x[0]*AT[i];
  }
  free(AT);

  /* One option */
  y[0] = 0;
  /* Sum up all the ypartials and scatter to the ranks */
  MPI_Reduce_scatter_block(ypartial, y, 1, MPI_DOUBLE, MPI_SUM, comm);

  /* Or */
#if 0
  /* Compute full y on all ranks */
  MPI_Allreduce(MPI_IN_PLACE, ypartial, 4, MPI_DOUBLE, MPI_SUM, comm);

  MPI_Comm_rank(comm, &rank);
  /* Pick out the bit we need. */
  y[0] = ypartial[rank];
#endif

  /* Alternate option would be to Allgather the vector (no need for a
     transpose of the matrix in this case) */
#if 0
  MPI_Allgather(x, 1, MPI_DOUBLE, ypartial, 1, MPI_DOUBLE, comm);
  y[0] = 0;
  for (int i = 0; i < n; i++) {
    y[0] += A[i]*ypartial[i];
  }
#endif
  free(ypartial);
}

static void print_vector(const double *x, MPI_Comm comm)
{
  int size, rank;
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  if (rank == 0) {
    double tmp;
    for (int i = 0; i < size; i++) {
      if (i != rank) {
        MPI_Recv(&tmp, 1, MPI_DOUBLE, i, 0, comm, MPI_STATUS_IGNORE);
      } else {
        tmp = x[0];
      }
      printf("%g\n", tmp);
    }
    printf("\n");
  } else {
    MPI_Ssend(x, 1, MPI_DOUBLE, 0, 0, comm);
  }
}

int main(int argc, char **argv)
{
  int rank;
  int size;
  double *A = NULL;
  double *AT = NULL;
  double x;
  double y;
  MPI_Comm comm;
  MPI_Init(&argc, &argv);

  comm = MPI_COMM_WORLD;

  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  A = malloc(size * sizeof(*A));
  AT = malloc(size * sizeof(*AT));

  x = rank + 1;
  for (int i = 0; i < size; i++) {
    A[i] = size*rank + i;
  }

  if (rank == 0) {
    printf("Matrix A is:\n\n");
  }
  print_matrix(A, size, comm);

  transpose_matrix(A, AT, size, comm);

  if (rank == 0) {
    printf("\nMatrix A^T is:\n\n");
  }

  print_matrix(AT, size, comm);

  if (rank == 0) {
    printf("\nVector x is:\n\n");
  }

  print_vector(&x, comm);
  matrix_vector_product(A, &x, &y, size, comm);

  if (rank == 0) {
    printf("\nVector y <- Ax is:\n\n");
  }

  print_vector(&y, comm);
  free(A);
  free(AT);
  MPI_Finalize();
}
