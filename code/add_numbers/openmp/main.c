
// This file is part of the HPC workshop 2019 at Durham University
// Author: Christian Arnold

/*
 * Your task is to parallelise the main computation using OpenMP.
 *
 * Note that the result of your calculation will suffer from floating point
 * roundoff errors, it might thus differ depending how you add the numbers.
 */

#ifdef _OPENMP
#include <omp.h>
#endif

#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * The program takes one argument: the size of the array of numbers to
 * perform compute on.
 */
int main(int argc, char *argv[]) {
  int n_threads;
  int n_numbers; /* the count of numbers in the file */

#ifdef _OPENMP
#pragma omp parallel
  { n_threads = omp_get_num_threads(); }
  printf("number of threads=%d \n", n_threads);
#else
  printf("Serial version\n");
#endif

  if (argc != 2) {
    fprintf(stderr, "Usage: %s N\n", argv[0]);
    fprintf(stderr, "\nPerform some computations on N random numbers.\n");
    return 1;
  }
  n_numbers = atoi(argv[1]);
  float *numbers = malloc(sizeof(*numbers) * n_numbers);
  for (int i = 0; i < n_numbers; i++) {
    /* random numbers in the interval (-1, 1) */
    numbers[i] = (float)(drand48() * 2. - 1.);
  }
  add_numbers(n_numbers, numbers);

  free(numbers);
  return 0;
}
