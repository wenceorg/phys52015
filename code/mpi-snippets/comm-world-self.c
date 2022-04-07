#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  int world_rank, world_size;
  int self_rank, self_size;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  MPI_Comm_rank(MPI_COMM_SELF, &self_rank);
  MPI_Comm_size(MPI_COMM_SELF, &self_size);

  printf("Hello, I am process %d of %d in COMM_WORLD; in COMM_SELF I am process %d of %d\n", world_rank, world_size, self_rank, self_size);

  MPI_Finalize();
  return 0;
}
