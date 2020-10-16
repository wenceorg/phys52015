#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "vec.h"
#include "mat.h"
#include "utils.h"

int CheckMatMult(Mat A, Vec x, Vec y)
{
  int ierr;
  int rank, size;
  int process_row;
  int error = 0;
  double expect;
  ierr = MPI_Comm_rank(A->comm, &rank);CHKERR(ierr);
  ierr = MPI_Comm_size(A->comm, &size);CHKERR(ierr);
  for (int i = 0; i < A->n; i++)
    for (int j = 0; j < A->n; j++)
      A->data[i*A->n + j] = rank + 1;

  for (int i = 0; i < x->n; i++)
    x->data[i] = size - rank;
  ierr = MatMult(A, x, y);CHKERR(ierr);

  process_row = rank / A->np;
  expect = 0;
  for (int i = 0; i < size; i++) {
    expect += (size - i)*(i / A->np + 1 + process_row * A->np);
  }
  expect *= x->n;

  for (int i = 0; i < y->n; i++) {
    if (fabs(y->data[i] - expect) > 1e-10) {
      fprintf(stderr, "[%d] CheckMatMult failed at local index %d, expected %g got %g\n", rank, i, expect, y->data[i]);
      error = 1;
    }
  }
  return error;
}

int CheckMatMatMult(Mat A, Mat B, Mat C, MatMultType algorithm)
{
  int ierr;
  int rank, size;
  int process_row, process_col;
  int error = 0;
  double expect;
  ierr = MPI_Comm_rank(A->comm, &rank);CHKERR(ierr);
  ierr = MPI_Comm_size(A->comm, &size);CHKERR(ierr);
  for (int i = 0; i < A->n; i++) {
    for (int j = 0; j < A->n; j++) {
      A->data[i*A->n + j] = rank + 1;
      B->data[i*B->n + j] = (size - rank);
      C->data[i*C->n + j] = size*rank + rank;
    }
  }
  
  ierr = MatMatMult(A, B, C, algorithm);CHKERR(ierr);

  process_row = rank / A->np;
  process_col = rank % A->np;

  expect = 0;
  for (int i = 0; i < A->np; i++) {
    expect += (i + 1 + process_row*A->np)*(size - i*A->np - process_col);
  }
  expect *= A->n;
  expect += size*rank + rank;
  for (int i = 0; i < C->n; i++) {
    for (int j = 0; j < C->n; j++) {
      if (fabs(C->data[i*C->n + j] - expect) > 1e-10) {
        fprintf(stderr, "[%d] CheckMatMatMult failed at local index (%d, %d), expected %g got %g\n", rank, i, j, expect, C->data[i*C->n + j]);
        error = 1;
      }
    }
  }
  return error;
}

