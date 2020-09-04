// HPC workshop - University of Durham
// author: Alejandro Benitez-Llambay
// data: November 2018
// purpose: demonstrate the advantages of using OpenMP for image processing

#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  struct Image myimage;
  struct Image output;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s INPUT OUTPUT\n", argv[0]);
    fprintf(stderr, "\nBlur the INPUT image and write to OUTPUT\n");
    fprintf(stderr, "Images should be in PPM format.\n");
    return 1;
  }
  read_ppm(argv[1], &myimage);

  int n = 10;

  output = blur_mean(myimage, n);

  write_ppm(argv[2], output);
  return 0;
}
