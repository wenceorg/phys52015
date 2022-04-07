#include <mpi.h>
#include <stdio.h>

static void ring_reduce(const int *sendbuf, int *recvbuf, MPI_Comm comm)
{
  /* Add my local contribution */
  recvbuf[0] = sendbuf[0];
  /* TODO implement the reduction */
  /* Hint, you can compute the left and right neighbours with modular
   * arithmetic:
   *
   * (x + y) % N;
   *
   * Produces is (x + y) mod N
   *
   * For subtraction, you should add the mod:
   *
   * (x - y + N) % N;
   */
}

int main(int argc, char **argv)
{
  int rank;
  int size;
  MPI_Comm comm;
  MPI_Init(&argc, &argv);

  comm = MPI_COMM_WORLD;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  int local_value = rank;

  int summed_value = 0;

  printf("[%d] Before reduction: local value is %d; summed value is %d\n",
         rank, local_value, summed_value);
  ring_reduce(&local_value, &summed_value, comm);

  printf("[%d] After reduction: local value is %d; summed value is %d\n",
         rank, local_value, summed_value);
  MPI_Finalize();
  return 0;
}
