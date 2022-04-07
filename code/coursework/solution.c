#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include "mat.h"
#include "vec.h"
#include "utils.h"

/* y <- Ax
 * - A: matrix
 * - x: input vector
 * - y: output vector
 */
int MatMult(Mat A, Vec x, Vec y)
{
  int ierr;
  if (A->N != x->N || A->N != y->N || x->n != A->n/A->np || x->n != y->n) {
    fprintf(stderr, "Mismatching sizes in MatMult %d %d %d\n", A->N, x->N, y->N);
    return MPI_Abort(A->comm, MPI_ERR_ARG);
  }
  fprintf(stderr, "[MatMult]: TODO, please implement me.\n");
  /* Do local part of multiplication. This is only correct in serial.
   * This code is included to show you how to call MatMultLocal,
   * you'll need to change the arguments in parallel.
   */
  ierr = MatMultLocal(x->n, A->data, x->data, y->data);CHKERR(ierr);
  return 0;
}

/* C <- AB + C using the SUMMA algorithm.
 *
 * - A: input matrix
 * - B: input matrix
 * - C: output matrix
 */
int MatMatMultSumma(Mat A, Mat B, Mat C)
{
  int ierr;
  fprintf(stderr, "[MatMatMultSumma]: TODO, please implement me.\n");
  /* Do local part of multiplication. Only correct in serial. */
  ierr = MatMatMultLocal(A->n, A->data, B->data, C->data);CHKERR(ierr);
  return 0;
}

/* C <- AB + C using Cannon's algorithm.
 *
 * - A: input matrix
 * - B: input matrix
 * - C: output matrix
 */
int MatMatMultCannon(Mat A, Mat B, Mat C)
{
  int ierr;
  fprintf(stderr, "[MatMatMultCannon]: TODO, please implement me.\n");
  /* Do local part of multiplication. Only correct in serial. */
  ierr = MatMatMultLocal(A->n, A->data, B->data, C->data);CHKERR(ierr);
  return 0;
}
