#include <mpi.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bench.h"
#include "check.h"
#include "vec.h"
#include "mat.h"
#include "utils.h"

typedef enum {CHECK_MAT_MULT, CHECK_MAT_MAT_MULT,
  BENCH_MAT_MULT, BENCH_MAT_MAT_MULT} Mode;

typedef struct {
  MatMultType algorithm;
  Mode mode;
  int N;
} UserOptions;

static void usage(const char *progname) {
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "%s -N N [-a ALGORITHM] [-t MODE] [-h]\n", progname);
  fprintf(stderr, "Run benchmarking or checking of matrix-vector or matrix-matrix multiplication.\n\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, " -h\n");
  fprintf(stderr, "    Print this help.\n");
  fprintf(stderr, " -a CANNON | SUMMA\n");
  fprintf(stderr, "    Select algorithm for matrix-matrix multiplication (default SUMMA).\n");
  fprintf(stderr, " -N N\n");
  fprintf(stderr, "    Set matrix size (required).\n");
  fprintf(stderr, " -t CHECK_MAT_MULT | BENCH_MAT_MULT | CHECK_MAT_MAT_MULT | BENCH_MAT_MAT_MULT\n");
  fprintf(stderr, "    Select execution mode (default CHECK_MAT_MAT_MULT).\n");
  fprintf(stderr, "    CHECK_MAT_MULT: check correctness of matrix-vector multiplication.\n");
  fprintf(stderr, "    BENCH_MAT_MULT: print timing data for matrix-vector multiplication.\n");
  fprintf(stderr, "        Prints min_time, max_time, avg_time, standard_deviation over all processes, in seconds.\n");
  fprintf(stderr, "    CHECK_MAT_MAT_MULT: check correctness of matrix-matrix multiplication.\n");
  fprintf(stderr, "    BENCH_MAT_MAT_MULT: print timing data for matrix-matrix multiplication.\n");
  fprintf(stderr, "        Prints min_time, max_time, avg_time, standard_deviation over all processes, in seconds.\n");
}
  
static int ProcessOptions(MPI_Comm comm, int argc, char **argv, UserOptions *options)
{
  int ch;
  int rank;
  int ierr;
  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);
  while ((ch = getopt(argc, argv, "a:t:N:h")) != -1) {
    switch (ch) {
    case 'a':
      if (strncmp(optarg, "CANNON", 6) == 0) {
        options->algorithm = MAT_MULT_CANNON;
      } else if (strncmp(optarg, "SUMMA", 5) == 0) {
        options->algorithm = MAT_MULT_SUMMA;
      } else {
        if (!rank) {
          fprintf(stderr, "Unrecognised algorithm type '%s'.\n\n", optarg);
          usage(argv[0]);
        }
        return 1;
      }
      break;
    case 't':
      if (strncmp(optarg, "CHECK_MAT_MULT", 14) == 0) {
        options->mode = CHECK_MAT_MULT;
      } else if (strncmp(optarg, "CHECK_MAT_MAT_MULT", 18) == 0) {
        options->mode = CHECK_MAT_MAT_MULT;
      } else if (strncmp(optarg, "BENCH_MAT_MULT", 14) == 0) {
        options->mode = BENCH_MAT_MULT;
      } else if (strncmp(optarg, "BENCH_MAT_MAT_MULT", 18) == 0) {
        options->mode = BENCH_MAT_MAT_MULT;
      } else {
        if (!rank) {
          fprintf(stderr, "Unrecognised execution mode '%s'.\n\n", optarg);
          usage(argv[0]);
        }
        return 1;
      }
      break;
    case 'N':
      errno = 0;
      options->N = (int)strtol(optarg, NULL, 10);
      if (!options->N || errno == ERANGE) {
        if (!rank) {
          fprintf(stderr, "Could not interpret matrix size '%s' as positive int.\n\n", optarg);
          usage(argv[0]);
        }
        return 1;
      }
      break;
    case 'h':
    default:
      if (!rank) usage(argv[0]);
      return 1;
    }
  }
  if (options->N < 0) {
    if (!rank) {
      fprintf(stderr, "Matrix size is a required argument please specify with -N.\n\n");
      usage(argv[0]);
    }
    return 1;
  }
  return 0;
}
        

int main(int argc, char **argv)
{
  MPI_Comm comm;
  int rank;
  int ierr;
  int check;
  UserOptions options = { .algorithm = MAT_MULT_SUMMA, .mode = CHECK_MAT_MULT, .N = -1 };
  Mat A, B, C;
  Vec x, y;

  ierr = MPI_Init(&argc, &argv);
  if (ierr) {
    fprintf(stderr, "MPI init failed with status code %d\n", ierr);
    return ierr;
  }
  comm = MPI_COMM_WORLD;
  if (ProcessOptions(comm, argc, argv, &options)) {
    ierr = MPI_Finalize();
    return ierr;
  }

  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);

  /* Create 2D process grid np x np (rows x cols) */
  ierr = MatCreate(comm, options.N, &A);CHKERR(ierr);
  ierr = MatCreate(comm, options.N, &B);CHKERR(ierr);
  ierr = MatCreate(comm, options.N, &C);CHKERR(ierr);
  ierr = VecCreate(comm, options.N, &x);CHKERR(ierr);
  ierr = VecCreate(comm, options.N, &y);CHKERR(ierr);

  switch (options.mode) {
  case CHECK_MAT_MULT:
    check = CheckMatMult(A, x, y);
    ierr = MPI_Allreduce(MPI_IN_PLACE, &check, 1, MPI_INT, MPI_MAX, A->comm);CHKERR(ierr);
    if (!rank) {
      if (check) {
        fprintf(stderr, "CheckMatMult failed.\n");
      } else {
        fprintf(stderr, "CheckMatMult succeeded.\n");
      }
    }
    break;
  case BENCH_MAT_MULT:
    ierr = BenchMatMult(A, x, y);CHKERR(ierr);
    break;
  case CHECK_MAT_MAT_MULT:
    check = CheckMatMatMult(A, B, C, options.algorithm);
    ierr = MPI_Allreduce(MPI_IN_PLACE, &check, 1, MPI_INT, MPI_MAX, A->comm);CHKERR(ierr);
    if (!rank) {
      if (check) {
        fprintf(stderr, "CheckMatMatMult failed.\n");
      } else {
        fprintf(stderr, "CheckMatMatMult succeeded.\n");
      }
    }
    break;
  case BENCH_MAT_MAT_MULT:
    ierr = BenchMatMatMult(A, B, C, options.algorithm);CHKERR(ierr);
    break;
  };

  ierr = MatDestroy(&A);CHKERR(ierr);
  ierr = MatDestroy(&B);CHKERR(ierr);
  ierr = MatDestroy(&C);CHKERR(ierr);
  ierr = VecDestroy(&x);CHKERR(ierr);
  ierr = VecDestroy(&y);CHKERR(ierr);
  ierr = MPI_Finalize();
  return ierr;
}
