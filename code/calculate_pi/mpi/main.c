// This file is part of the HPC workshop 2019 at Durham University
// Author: Christian Arnold

/*
 * This program calculates the numerical value of the constant PI.
 */

#include "proto.h"
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.141592653589793
#endif
/*
 * The program takes one argument: the name of the file containing the
 * numbers in binary format.
 */
int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  // Find out size
  int n_tasks;
  MPI_Comm_size(MPI_COMM_WORLD, &n_tasks);

  // Find out rank
  int this_task;
  MPI_Comm_rank(MPI_COMM_WORLD, &this_task);

  printf("Running on %i tasks, this task %i\n", n_tasks, this_task);

  clock_t start, end;

  int N; /* the total count of random coordinates vectors */
  if (argc != 2) {
    if (this_task == 0) {
      fprintf(stderr, "Usage: %s N\n", argv[0]);
      fprintf(stderr, "\nEstimate PI by Monte-Carlo using N samples\n");
    }
    MPI_Finalize();
    return 1;
  }
  N = atoi(argv[1]) / n_tasks;

  start = clock();

  double my_pi = calculate_pi(N);

  end = clock();

  double time = ((double)end - start) / CLOCKS_PER_SEC;

  double error = M_PI - my_pi;

  if (this_task == 0) {
    printf("The value of Pi is %.20f, the error is  %.20f\n", my_pi, error);
    printf("The calculation took %g s\n", time);
  }

  MPI_Finalize();
  return 0;
}
