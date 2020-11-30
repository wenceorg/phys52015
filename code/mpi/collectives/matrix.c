#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

static void print_matrix(const double *A, int n, MPI_Comm comm)
{
  /* Implement printing of A by gathering to rank 0 and printing there. */
}

static void transpose_matrix(const double *A, double *AT, int n, MPI_Comm comm)
{
  /* Implement AT <- transpose(A) (MPI_Alltoall may be helpful) */
}

static void matrix_vector_product(const double *A, const double *x, double *y, int n, MPI_Comm comm)
{
  /* Implement y <- Ax, it might be useful to tranpose A first (think
     about the data distribution). */
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
