#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bench.h"
#include "vec.h"
#include "mat.h"
#include "utils.h"

static int ShowTimingStats(MPI_Comm comm, double duration, const char *desc)
{
  int ierr;
  int rank, size;
  double dmin, dmax, dmean, dsigma;
  
  ierr = MPI_Comm_size(comm, &size);CHKERR(ierr);
  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);
  ierr = MPI_Allreduce(&duration, &dmin, 1, MPI_DOUBLE, MPI_MIN, comm);CHKERR(ierr);
  ierr = MPI_Allreduce(&duration, &dmax, 1, MPI_DOUBLE, MPI_MAX, comm);CHKERR(ierr);
  ierr = MPI_Allreduce(&duration, &dmean, 1, MPI_DOUBLE, MPI_SUM, comm);CHKERR(ierr);
  dmean = dmean / size;
  dsigma = (duration - dmean) * (duration - dmean);
  ierr = MPI_Allreduce(MPI_IN_PLACE, &dsigma, 1, MPI_DOUBLE, MPI_SUM, comm);CHKERR(ierr);
  if (size > 1) {
    dsigma = sqrt(dsigma/(size - 1));
  } else {
    dsigma = 0;
  }
  if (!rank) {
    printf("TestCase: Min Max Mean Std\n");
    printf("%s: %g %g %g %g\n", desc, dmin, dmax, dmean, dsigma);
  }
  return 0;
}

int BenchMatMult(Mat A, Vec x, Vec y)
{
  int ierr;
  double start, end;
  int rank, size;
  ierr = MPI_Comm_rank(A->comm, &rank);CHKERR(ierr);
  ierr = MPI_Comm_size(A->comm, &size);CHKERR(ierr);
  srand48((long)(size * rank + rank));
  for (int i = 0; i < A->n; i++)
    for (int j = 0; j < A->n; j++)
      A->data[i*A->n + j] = drand48();
  for (int i = 0; i < x->n; i++)
    x->data[i] = drand48();
  start = MPI_Wtime();
  ierr = MatMult(A, x, y);CHKERR(ierr);
  end = MPI_Wtime();
  ierr = ShowTimingStats(A->comm, end - start, "MatMult");CHKERR(ierr);
  return 0;
}

int BenchMatMatMult(Mat A, Mat B, Mat C, MatMultType algorithm)
{
  int ierr;
  double start, end;
  const char *desc = NULL;
  int rank, size;
  ierr = MPI_Comm_rank(A->comm, &rank);CHKERR(ierr);
  ierr = MPI_Comm_size(A->comm, &size);CHKERR(ierr);
  srand48((long)(size * rank + rank));
  for (int i = 0; i < A->n; i++)
    for (int j = 0; j < A->n; j++)
      A->data[i*A->n + j] = drand48();
  for (int i = 0; i < B->n; i++)
    for (int j = 0; j < B->n; j++)
      B->data[i*B->n + j] = drand48();
  for (int i = 0; i < C->n; i++)
    for (int j = 0; j < C->n; j++)
      C->data[i*C->n + j] = drand48();

  start = MPI_Wtime();
  ierr = MatMatMult(A, B, C, algorithm);CHKERR(ierr);
  end = MPI_Wtime();
  switch (algorithm) {
  case MAT_MULT_SUMMA:
    desc = "MatMatMult[SUMMA]";
    break;
  case MAT_MULT_CANNON:
    desc = "MatMatMult[CANNON]";
    break;
  }
  ierr = ShowTimingStats(A->comm, end - start, desc);CHKERR(ierr);
  return 0;
}
