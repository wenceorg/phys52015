#ifndef _UTILS_H
#define _UTILS_H
#include <mpi.h>
#include <stdio.h>
#include "vec.h"
#include "mat.h"
#define CHKERR(ierr) do { if (ierr) { fprintf(stderr, "MPI failed with return code %d\n", ierr); return MPI_Abort(MPI_COMM_WORLD, ierr); } } while (0)

typedef enum {CHECK_MAT_MULT, CHECK_MAT_MAT_MULT,
  BENCH_MAT_MULT, BENCH_MAT_MAT_MULT} Mode;

typedef struct {
  MatMultType algorithm;
  Mode mode;
  int N;
  const char *filename;
} UserOptions;

#endif
