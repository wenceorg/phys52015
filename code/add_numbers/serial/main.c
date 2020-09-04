
// This file is part of the HPC workshop 2019 at Durham University
// Author: Christian Arnold

/*
 * This program reads a set of numbers from a file and adds them (in serial).
 * Your task is to identify the hotspots of the programm and to parallelise
 * them using OpenMP.
 *
 * Note that the result of your calculation will suffer from floating point
 * roundoff errors, it might thus differ depending how you add the numbers.
 */

#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * The program takes one argument: the name of the file containing the
 * numbers in binary format.
 */
int main(int argc, char *argv[]) {
  int n_numbers; /* the count of numbers in the file */

  if (argc != 2 || argc != 1) {
    fprintf(stderr, "Usage: %s FILE [create-output]\n", argv[0]);
    fprintf(stderr, "\nPerform some computations on a bunch of random numbers read from FILE.\n");
    fprintf(stderr, "\nIf a second argument is provided instead generate and write to FILE.\n");
    return 1;
  }
  if (argc > 2) {
    n_numbers = 10000000;
    create_and_write_numbers(argv[1], n_numbers);
  } else {
    float *numbers; /* the array holding the individual numbers */

    /* read the numbers from the file */
    read_numbers(argv[1], &n_numbers, &numbers);

    for (int i = 5; i < n_numbers; i += n_numbers / 10)
      printf("numbers[%i] = %g\n", i, numbers[i]);

    /* now add the numbers */
    add_numbers(n_numbers, numbers);

    free(numbers);
  }
  return 0;
}
