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

static void usage(const char *progname) {
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "%s -N N [-a ALGORITHM] [-t MODE] [-f FILE] [-h]\n", progname);
  fprintf(stderr, "Run benchmarking or checking of matrix-vector or matrix-matrix multiplication.\n\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, " -N N\n");
  fprintf(stderr, "    Set matrix size (required).\n\n");
  fprintf(stderr, " -a CANNON | SUMMA\n");
  fprintf(stderr, "    Select algorithm for matrix-matrix multiplication (default SUMMA).\n\n");
  fprintf(stderr, " -t CHECK_MAT_MULT | BENCH_MAT_MULT | CHECK_MAT_MAT_MULT | BENCH_MAT_MAT_MULT\n");
  fprintf(stderr, "    Select execution mode (default CHECK_MAT_MAT_MULT).\n");
  fprintf(stderr, "    CHECK_MAT_MULT: check correctness of matrix-vector multiplication.\n");
  fprintf(stderr, "    BENCH_MAT_MULT: print timing data for matrix-vector multiplication.\n");
  fprintf(stderr, "    CHECK_MAT_MAT_MULT: check correctness of matrix-matrix multiplication.\n");
  fprintf(stderr, "    BENCH_MAT_MAT_MULT: print timing data for matrix-matrix multiplication.\n\n");
  fprintf(stderr, " -f FILE\n");
  fprintf(stderr, "    In benchmarking mode, print timing data to FILE in JSON format.\n");
  fprintf(stderr, "    WARNING: overwrites output file if it exists.\n");
  fprintf(stderr, "    Use \"-f -\" to dump to standard output.\n\n");  
  fprintf(stderr, " -h\n");
  fprintf(stderr, "    Print this help.\n");
}
  
static int ProcessOptions(MPI_Comm comm, int argc, char **argv, UserOptions *options)
{
  int ch;
  int rank;
  int ierr;
  ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);
  while ((ch = getopt(argc, argv, "a:t:N:f:h")) != -1) {
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
    case 'f':
      options->filename = strdup(optarg);
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
  UserOptions options = { .algorithm = MAT_MULT_SUMMA, .mode = CHECK_MAT_MULT, .N = -1, .filename = NULL };

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

  switch (options.mode) {
  case CHECK_MAT_MULT:
    check = CheckMatMult(comm, options);
    ierr = MPI_Allreduce(MPI_IN_PLACE, &check, 1, MPI_INT, MPI_MAX, comm);CHKERR(ierr);
    if (!rank) {
      if (check) {
        fprintf(stderr, "CheckMatMult failed.\n");
      } else {
        fprintf(stderr, "CheckMatMult succeeded.\n");
      }
    }
    break;
  case BENCH_MAT_MULT:
    ierr = BenchMatMult(comm, options);CHKERR(ierr);
    break;
  case CHECK_MAT_MAT_MULT:
    check = CheckMatMatMult(comm, options);
    ierr = MPI_Allreduce(MPI_IN_PLACE, &check, 1, MPI_INT, MPI_MAX, comm);CHKERR(ierr);
    if (!rank) {
      if (check) {
        fprintf(stderr, "CheckMatMatMult failed.\n");
      } else {
        fprintf(stderr, "CheckMatMatMult succeeded.\n");
      }
    }
    break;
  case BENCH_MAT_MAT_MULT:
    ierr = BenchMatMatMult(comm, options);CHKERR(ierr);
    break;
  };
  free((void *)options.filename);
  ierr = MPI_Finalize();
  return ierr;
}
