// HPC workshop - University of Durham
// author: Alejandro Benitez-Llambay
// data: November 2018

#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void free_image(struct Image *image)
{
  free(image->r);
  free(image->g);
  free(image->b);
}

int main(int argc, char *argv[]) {
  struct Image myimage = {0};
  struct Image output = {0};

  if (argc != 3) {
    fprintf(stderr, "Usage: %s INPUT OUTPUT\n", argv[0]);
    fprintf(stderr, "\nBlur the INPUT image and write to OUTPUT\n");
    fprintf(stderr, "Images should be in PPM format.\n");
    return 1;
  }
  read_ppm(argv[1], &myimage);

  int n = 1;

  blur_mean(myimage, n, &output);

  write_ppm(argv[2], output);
  free_image(&myimage);
  free_image(&output);
  return 0;
}
