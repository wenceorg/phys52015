
// This file is part of the HPC workshop of Durham University
// Prepared by Alejandro Benitez-Llambay, November 2018
// email: alejandro.b.llambay@durham.ac.uk

#ifdef _OPENMP
#include <omp.h>
#endif

#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  struct Image myimage;
  int n_threads;
  struct Image output;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s INPUT OUTPUT\n", argv[0]);
    fprintf(stderr, "\nBlur the INPUT image and write to OUTPUT\n");
    fprintf(stderr, "Images should be in PPM format.\n");
    return 1;
  }

#ifdef _OPENMP
#pragma omp parallel
  { n_threads = omp_get_num_threads(); }
  printf("number of threads=%d \n", n_threads);
#else
  printf("Serial version\n");
#endif

  read_ppm(argv[1], &myimage);

  int n = 10;

  output = blur_mean_automatic(myimage, n);

  write_ppm(argv[2], output);
  return 0;
}
