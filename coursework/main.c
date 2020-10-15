#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vec.h"
#include "mat.h"
#include "utils.h"

int main(int argc, char **argv)
{
  MPI_Comm comm;
  int rank;
  int N = 4;                  /* global matrix size */
  int ierr;
  Mat A, B, C;
  Vec x, y;

  ierr = MPI_Init(&argc, &argv);
  if (ierr) {
    fprintf(stderr, "MPI init failed with status code %d\n", ierr);
    return ierr;
  }

  comm = MPI_COMM_WORLD;
  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);

  /* Create 2D process grid np x np (rows x cols) */
  ierr = MatCreate(comm, N, &A);CHKERR(ierr);
  ierr = MatCreate(comm, N, &B);CHKERR(ierr);
  ierr = MatCreate(comm, N, &C);CHKERR(ierr);
  ierr = VecCreate(comm, N, &x);CHKERR(ierr);
  ierr = VecCreate(comm, N, &y);CHKERR(ierr);

  /* A test case making a representation of the 1D laplacian
     (tremendously inefficiently). */
  for (int i = 0; i < A->n; i++) {
    int gir = (rank / A->np) * A->n + i;
    for (int j = 0; j < A->n; j++) {
      int gic = (rank % A->np) * A->n + j;
      if (gir == gic) {
        /* diagonal */
        ierr = MatSetValue(A, i, j, 1);CHKERR(ierr);
        ierr = MatSetValue(B, i, j, 1);CHKERR(ierr);
      } else if (gic == gir + 1) {
        /* super-diagonal */
        ierr = MatSetValue(A, i, j, 1);CHKERR(ierr);
      } else if (gic == gir - 1) {
        /* sub-diagonal */
        ierr = MatSetValue(B, i, j, 1);CHKERR(ierr);
      }
      ierr = MatSetValue(A, i, j, rank + 1);CHKERR(ierr);
      ierr = MatSetValue(B, i, j, rank + 1);CHKERR(ierr);
    }
  }
  for (int i = 0; i < x->n; i++) {
    x->data[i] = rank + 1;
  }

  /* C <- A * B + C */
  /* Timing routines. */
  /*
   * ierr = matmatmult_cannon(A, B, C);CHKERR(ierr);
   */

  ierr = MatMult(A, x, y);CHKERR(ierr);
  ierr = MatView(A, NULL);CHKERR(ierr);
  ierr = VecView(x, NULL);CHKERR(ierr);
  ierr = VecView(y, NULL);CHKERR(ierr);
  ierr = MatDestroy(&A);CHKERR(ierr);
  ierr = MatDestroy(&B);CHKERR(ierr);
  ierr = MatDestroy(&C);CHKERR(ierr);
  ierr = VecDestroy(&x);CHKERR(ierr);
  ierr = VecDestroy(&y);CHKERR(ierr);
  ierr = MPI_Finalize();
  return ierr;
}
