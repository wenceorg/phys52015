#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>

void Gather_to_zero(const int *send, int **gathered, MPI_Comm comm)
{
  int size, rank;

  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  int *recvbuf = malloc(size * sizeof(*recvbuf));
  if (rank == 0) {
    MPI_Request *requests;

    requests = malloc((size-1) * sizeof(*requests));
    recvbuf[0] = send[0];
    for (int i = 1; i < size; i++) {
      MPI_Irecv(&(recvbuf[i]), 1, MPI_INT, i, 0, comm, &(requests[i-1]));
    }
    MPI_Waitall(size-1, requests, MPI_STATUSES_IGNORE);
    free(requests);
  } else {
    MPI_Send(send, 1, MPI_INT, 0, 0, comm);
  }
  *gathered = recvbuf;
}

void Gather_to_zero_blocking(const int *send, int **gathered, MPI_Comm comm)
{
  int size, rank;

  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  int *recvbuf = malloc(size * sizeof(*recvbuf));
  if (rank == 0) {
    /* Put my value in the first spot */
    recvbuf[0] = *send;
    for (int i = 1; i < size; i++) {
      /* Receive from each process in turn */
      MPI_Recv(&(recvbuf[i]), 1, MPI_INT, i, 0, comm, MPI_STATUS_IGNORE);
    }
  } else {
    /* Send to rank 0 */
    MPI_Send(send, 1, MPI_INT, 0, 0, comm);
  }
  *gathered = recvbuf;
}

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  MPI_Comm comm = MPI_COMM_WORLD;
  int local_value[2];
  int rank, size;
  int *gathered = NULL;
  double start, end;

  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  local_value[0] = rank * rank;
  local_value[1] = size - rank;
  /*
   * Process 0: A
   * Process 1: B
   * Process 2: C
   * Process 3: D

   * Gather_to_zero(...) ->
   *
   * Process 0: A, B, C, D
   */

  start = MPI_Wtime();
  Gather_to_zero(local_value, &gathered, comm);
  end = MPI_Wtime();
  if (rank == 0) {
    printf("Non-blocking gather takes %.3g s\n", end - start);
    free(gathered);
  }
  start = MPI_Wtime();
  Gather_to_zero_blocking(local_value, &gathered, comm);
  end = MPI_Wtime();
  if (rank == 0) {
    printf("Blocking gather takes %.3g s\n", end - start);
    free(gathered);
  }
  MPI_Finalize();
  return 0;
}
