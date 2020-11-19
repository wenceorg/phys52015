#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  int rank, size;

  double value = 1;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0) {
    value = 10;
  }

  if (rank == 0) {
    MPI_Send(&value, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
  } else {
    printf("[%d]: before receiving, my value is %g\n", rank, value);
    MPI_Recv(&value, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  printf("[%d]: my value is %g\n", rank, value);
  MPI_Finalize();
  return 0;
}
