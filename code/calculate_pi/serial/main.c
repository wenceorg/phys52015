
// This file is part of the HPC workshop 2019 at Durham University
// Author: Christian Arnold

/*
 * This program calculates the numerical value of the constant PI.
 */

#include "proto.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef M_PI
#define M_PI 3.141592653589793
#endif
/*
 * The program takes one argument: the number of samples to use.
 */
int main(int argc, char *argv[]) {
  clock_t start, end;

  int N; /* the total count of random coordinates vectors */
  if (argc != 2) {
    fprintf(stderr, "Usage: %s N\n", argv[0]);
    fprintf(stderr, "\nEstimate PI by Monte-Carlo using N samples\n");
    return 1;
  }
  N = atoi(argv[1]);

  start = clock();

  double my_pi = calculate_pi(N);

  end = clock();

  double time = ((double)end - start) / CLOCKS_PER_SEC;

  double error = M_PI - my_pi;
  printf("The value of Pi is %.20f, the error is  %.20f\n", my_pi, error);
  printf("The calculation took %g s\n", time);

  return 0;
}
