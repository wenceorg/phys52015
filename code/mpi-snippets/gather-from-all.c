#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>

void gather_nonblocking(const int *send, int *recvbuf, MPI_Comm comm)
{
  int size, rank;

  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

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
}

void gather_blocking(const int *send, int *recvbuf, MPI_Comm comm)
{
  int size, rank;

  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

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
}

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);

  MPI_Comm comm = MPI_COMM_WORLD;
  int local;
  int rank, size;
  int *blocking = NULL;
  int *nonblocking = NULL;
  double start, end;

  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  local = rank * rank;
  /*
   * Process 0: A
   * Process 1: B
   * Process 2: C
   * Process 3: D
   *
   * gather_...(...) ->
   * Process 0: A, B, C, D
   */

  if (rank == 0) {
    /* Allocate space for output arrays.
     * 1 int per process.
     */
    blocking = malloc(size*sizeof(*blocking));
    nonblocking = malloc(size*sizeof(*nonblocking));
  }

  start = MPI_Wtime();
  for (int i = 0; i < 100; i++) {
    gather_blocking(&local, blocking, comm);
  }
  end = MPI_Wtime();
  if (rank == 0) {
    printf("Blocking gather takes %.3g s\n", (end - start)/100);
  }

  start = MPI_Wtime();
  for (int i = 0; i < 100; i++) {
    gather_nonblocking(&local, nonblocking, comm);
  }
  end = MPI_Wtime();
  if (rank == 0) {
    printf("Non-blocking gather takes %.3g s\n", (end - start)/100);
  }

  free(blocking);
  free(nonblocking);
  MPI_Finalize();
  return 0;
}
