#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bench.h"
#include "vec.h"
#include "mat.h"
#include "utils.h"

static int TimingStats(MPI_Comm comm, double duration,
                       double *data)
{
  int ierr;
  int size;
  
  ierr = MPI_Comm_size(comm, &size);CHKERR(ierr);
  ierr = MPI_Allreduce(&duration, &data[0], 1, MPI_DOUBLE, MPI_MIN, comm);CHKERR(ierr);
  ierr = MPI_Allreduce(&duration, &data[1], 1, MPI_DOUBLE, MPI_MAX, comm);CHKERR(ierr);
  ierr = MPI_Allreduce(&duration, &data[2], 1, MPI_DOUBLE, MPI_SUM, comm);CHKERR(ierr);
  data[2] /= size;
  data[3] = (duration - data[2]) * (duration - data[2]);
  ierr = MPI_Allreduce(MPI_IN_PLACE, &data[3], 1, MPI_DOUBLE, MPI_SUM, comm);CHKERR(ierr);
  if (size > 1) {
    data[3] = sqrt(data[3]/(size - 1));
  } else {
    data[3] = 0;
  }
  return 0;
}

int BenchMatMult(MPI_Comm comm, const UserOptions options)
{
  int ierr;
  double start, end;
  int rank, size;
  double timing[4];
  Mat A;
  Vec x, y;

  ierr = MatCreate(comm, options.N, &A);CHKERR(ierr);
  ierr = VecCreate(comm, options.N, &x);CHKERR(ierr);
  ierr = VecCreate(comm, options.N, &y);CHKERR(ierr);  

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
  ierr = TimingStats(A->comm, end - start, timing);CHKERR(ierr);

  if (!rank) {
    if (options.filename) {
      int isstdout = !strcmp(options.filename, "-");
      FILE * fd = isstdout ? stdout : fopen(options.filename, "w");
      if (!fd) {
        fprintf(stderr, "Unable to open %s for writing\n", options.filename);
        return MPI_Abort(A->comm, MPI_ERR_ARG);
      }
      fprintf(fd, "{\"TestCase\": \"MatMult\", \"nprocs\": %d, \"N\": %d, "
              "\"min\": %g, \"max\": %g, \"mean\": %g, \"std\": %g}\n",
              size, A->N, timing[0], timing[1], timing[2], timing[3]);
      if (!isstdout && fclose(fd)) {
        fprintf(stderr, "Unable to close %s after writing\n", options.filename);
      } else if (!isstdout) {
        printf("Timing data saved to %s\n", options.filename);
      }
    } else {
      printf("Timing data for MatMult on %d processes, matrix size %d\n",
             size, A->N);
      printf("All data in seconds. Min, Max, Mean, Standard deviation.\n");
      printf("%g %g %g %g\n", timing[0], timing[1], timing[2], timing[3]);
    }
  }

  ierr = MatDestroy(&A);CHKERR(ierr);
  ierr = VecDestroy(&x);CHKERR(ierr);
  ierr = VecDestroy(&y);CHKERR(ierr);
  return 0;
}

int BenchMatMatMult(MPI_Comm comm, const UserOptions options)
{
  int ierr;
  double start, end;
  const char *desc = NULL;
  int rank, size;
  double timing[4];
  Mat A, B, C;
  ierr = MatCreate(comm, options.N, &A);CHKERR(ierr);
  ierr = MatCreate(comm, options.N, &B);CHKERR(ierr);
  ierr = MatCreate(comm, options.N, &C);CHKERR(ierr);

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
  ierr = MatMatMult(A, B, C, options.algorithm);CHKERR(ierr);
  end = MPI_Wtime();
  switch (options.algorithm) {
  case MAT_MULT_SUMMA:
    desc = "MatMatMult[SUMMA]";
    break;
  case MAT_MULT_CANNON:
    desc = "MatMatMult[CANNON]";
    break;
  }
  ierr = TimingStats(A->comm, end - start, timing);CHKERR(ierr);
  if (!rank) {
    if (options.filename) {
      int isstdout = !strcmp(options.filename, "-");
      FILE * fd = isstdout ? stdout : fopen(options.filename, "w");
      if (!fd) {
        fprintf(stderr, "Unable to open %s for writing\n", options.filename);
        return MPI_Abort(A->comm, MPI_ERR_ARG);
      }
      fprintf(fd, "{\"TestCase\": \"%s\", \"nprocs\": %d, \"N\": %d, "
              "\"min\": %g, \"max\": %g, \"mean\": %g, \"std\": %g}\n",
              desc, size, A->N, timing[0], timing[1], timing[2], timing[3]);
      if (!isstdout && fclose(fd)) {
        fprintf(stderr, "Unable to close %s after writing\n", options.filename);
      } else if (!isstdout) {
        printf("Timing data saved to %s\n", options.filename);
      }
    } else {
      printf("Timing data for %s on %d processes, matrix size %d\n",
             desc, size, A->N);
      printf("All data in seconds. Min, Mean, Max, Standard deviation.\n");
      printf("%g %g %g %g\n", timing[0], timing[1], timing[2], timing[3]);
    }
  }
  ierr = MatDestroy(&A);CHKERR(ierr);
  ierr = MatDestroy(&B);CHKERR(ierr);
  ierr = MatDestroy(&C);CHKERR(ierr);

  return 0;
}
