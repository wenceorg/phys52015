// HPC workshop - University of Durham
// author: Alejandro Benitez-Llambay
// data: November 2018
#ifndef _PROTO_H
#define _PROTO_H

struct Image {
  int dimx;
  int dimy;
  float *r;
  float *g;
  float *b;
};

void read_ppm(char *filename, struct Image *image);
void write_ppm(char *filename, struct Image image);
void blur_mean(struct Image input, int n, struct Image *output);

#endif
