
// This file is part of the HPC workshop of Durham University
// Prepared by Alejandro Benitez-Llambay, November 2018
// email: alejandro.b.llambay@durham.ac.uk
#ifndef _PROTO_H
#define _PROTO_H

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

struct Image {
  int dimx;
  int dimy;
  float *r;
  float *g;
  float *b;
  int *xcoord;
  int *ycoord;
};

void read_ppm(char *filename, struct Image *image);
void write_ppm(char *filename, struct Image image);
struct Image blur_mean_manual(struct Image input, int n);
struct Image blur_mean_automatic(struct Image input, int n);

#endif
