#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

static void ping_pong(void *buffer, int count, MPI_Datatype dtype, MPI_Comm comm)
{
  /* Implement a ping pong.
   *
   * rank 0 should send count bytes from buffer to rank 1
   * rank 1 should then send the received data back to rank 0
   *
   */

  int rank, size;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  if (rank == 0) {
    MPI_Send(buffer, count, dtype, 1, 0, comm);
    MPI_Recv(buffer, count, dtype, 1, 0, comm, MPI_STATUS_IGNORE);
  } else if (rank == 1) {
    MPI_Recv(buffer, count, dtype, 0, 0, comm, MPI_STATUS_IGNORE);
    MPI_Send(buffer, count, dtype, 0, 0, comm);
  } else {
    /* Nothing to do */
  }
}

int main(int argc, char **argv)
{
  int nbytes, rank;
  int nreps;
  char *buffer;
  double start, end;
  MPI_Comm comm;

  MPI_Init(&argc, &argv);

  comm = MPI_COMM_WORLD;
  MPI_Comm_rank(comm, &rank);
  nbytes = argc > 1 ? atoi(argv[1]) : 1;

  buffer = calloc(nbytes, sizeof(*buffer));

  start = MPI_Wtime();
  for (int i = 0; i < 100; i++) {
    ping_pong(buffer, nbytes, MPI_CHAR, comm);
  }
  end = MPI_Wtime();

  /* Figure out how many repetitions to do so that we measure about 3
     seconds of total time */
  nreps = (int)(300 / (end - start));
  if (nreps <= 0) {
    nreps = 1;
  }
  /* We might not have agreed on the total time, so pick the maximum
     with allreduce */
  MPI_Allreduce(MPI_IN_PLACE, &nreps, 1, MPI_INT, MPI_MAX, comm);

  start = MPI_Wtime();
  for (int i = 0; i < nreps; i++) {
    ping_pong(buffer, nbytes, MPI_CHAR, comm);
  }
  end = MPI_Wtime();
  if (rank == 0) {
    /* Time for one message is half a pingpong */
    printf("%d %d %g\n", nbytes, nreps, (end - start)/(nreps*2));
  }
  free(buffer);
  MPI_Finalize();
  return 0;
}
