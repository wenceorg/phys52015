// This file is part of the HPC workshop 2019 at Durham University
// Author: Christian Arnold
#define _XOPEN_SOURCE
#include <mpi.h>

#include "proto.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* This function draws two random numbers, x and y,
 * from the interval [0, 1) and checks if the corresponding
 * coordinates are within the unit circle, i.e.
 * x^2 + y^2 < 1
 * retuns 1 if true, otherwise 0
 */
int create_and_check_coordinates(void) {
  double x, y;

  x = drand48();
  y = drand48();

  double r_sq = x * x + y * y;

  if (r_sq < 1) /* coordinates are within the circle */
    return 1;

  return 0;
}

/* This function performs N checks and returns
 * the number of coordinate pairs which are within the circle.
 */
int do_checks(int N) {
  int N_in = 0;
  for (int i = 0; i < N; i++)
    N_in += create_and_check_coordinates();

  return N_in;
}

/* This function calculates pi and returns the calculated value given a certain
 * number of random tries */
double calculate_pi(int N) {
  int this_task;
  MPI_Comm_rank(MPI_COMM_WORLD, &this_task);
  /* seed the random number generator */
  srand48(this_task);

  int N_in = do_checks(N);

  int N_in_global;
  int N_global;

  printf("Task %i: calculating pi using %i random points, in are %i\n",
         this_task, N, N_in);

  /* now sum up the results across the different tasks */
  MPI_Allreduce(&N_in, &N_in_global, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&N, &N_global, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  double my_pi = 4.0 * N_in_global / N_global;
  return my_pi;
}
