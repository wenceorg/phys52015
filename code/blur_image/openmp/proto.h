
// This file is part of the HPC workshop of Durham University
// Prepared by Alejandro Benitez-Llambay, November 2018
// email: alejandro.b.llambay@durham.ac.uk
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
void blur_mean(struct Image input, int n, struct Image *image);

#endif
