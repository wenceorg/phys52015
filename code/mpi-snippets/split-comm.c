#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  int world_rank, world_size;
  int split_rank, split_size;
  MPI_Comm comm, splitcomm;

  MPI_Init(&argc, &argv);

  comm = MPI_COMM_WORLD;
  MPI_Comm_rank(comm, &world_rank);
  MPI_Comm_size(comm, &world_size);

  /* Split into even and odd ranks */
  MPI_Comm_split(comm, world_rank % 2, world_rank, &splitcomm);

  MPI_Comm_rank(splitcomm, &split_rank);
  MPI_Comm_size(splitcomm, &split_size);

  if (world_rank % 2 == 0) {
    int result = 0;
    /* Even processes do an allreduce with MPI_SUM */
    MPI_Allreduce(&world_rank, &result, 1, MPI_INT, MPI_SUM, splitcomm);
    /* Serialise output */
    for (int i = 0; i < split_size; i++) {
      if (split_rank == i) {
        printf("world_rank: %d; splitcomm size: %d, rank %d. sum: %d\n",
               world_rank, split_size, split_rank, result);
        fflush(stdout);
      }
      MPI_Barrier(splitcomm);
    }
      
    /* Barrier to ensure that all even rank prints happen before odd rank prints. */
    MPI_Barrier(comm);
  } else {
    /* Odd processes do an Allgather */
    int *result = malloc(split_size * sizeof(*result));
    /* Barrier to ensure that all even rank prints happen before odd rank prints. */
    MPI_Barrier(comm);
    MPI_Allgather(&world_rank, 1, MPI_INT,
                  result, 1, MPI_INT, splitcomm);

    /* Serialise output */
    for (int i = 0; i < split_size; i++) {
      if (split_rank == i) {
        printf("world_rank: %d; splitcomm size: %d, rank %d. gathered ranks: ",
               world_rank, split_size, split_rank);
        for (int i = 0; i < split_size; i++) {
          printf("%d ", result[i]);
        }
        printf("\n");
        fflush(stdout);
      }
      MPI_Barrier(splitcomm);
    }
    free(result);
  }
  MPI_Comm_free(&splitcomm);
  MPI_Finalize();
  return 0;
}
