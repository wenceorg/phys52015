#include <mpi.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "pgmio.h"


void ReadImage(const char *filename, Image *image)
{
  Image im;
  void *data;
  FILE *fp;
  int nread;
  int nx, ny, threshold;
  int format;

  CreateImage(&im);

  fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "ReadImage: unable to open '%s'\n", filename);
    MPI_Abort(MPI_COMM_WORLD, 1);
    return;
  }

  nread = fscanf(fp, "P%d", &format);
  if (format != 5 || nread != 1) {
    fprintf(stderr, "Expecting PGM format version 5\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
    return;
  }

  nread = fscanf(fp, "%d %d", &nx, &ny);
  if (nread != 2) {
    fprintf(stderr, "Unable to read image size\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
    return;
  }
  SetSizes(im, nx, ny);

  nread = fscanf(fp, "%d\n", &threshold);
  if (nread != 1) {
    fprintf(stderr, "Unable to read image threshold\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
    return;
  }
  SetThreshold(im, threshold);

  if (im->threshold > 0 && im->threshold < 256) {
    data = malloc(im->NX*im->NY);
  } else if (im->threshold > 0 && im->threshold < 65536) {
    data = malloc(im->NX*im->NY*2);
  } else {
    fprintf(stderr, "Invalid image maxval, should be in range (0, 65536), is %d\n", im->threshold);
    MPI_Abort(MPI_COMM_WORLD, 1);
    return;
  }

  size_t nbytes = fread(data, im->threshold < 256 ? 1 : 2,
                        (size_t)(im->NX*im->NY),
                        fp);
  fclose(fp);
  if (nbytes != (size_t)(im->NX*im->NY)) {
    fprintf(stderr, "Unable to read image data\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
    return;
  }

  if (im->threshold < 256) {
    const uint8_t *image = (const uint8_t *)data;
    for (int i = 0; i < im->NX * im->NY; i++) {
      im->data[i] = (float)image[i] / (float)(im->threshold);
    }
  } else {
    const uint16_t *image = (const uint16_t *)data;
    for (int i = 0; i < im->NX * im->NY; i++) {
      im->data[i] = (float)image[i] / (float)(im->threshold);
    }
  }
  free(data);

  *image = im;
  return;
}

void WriteImage(const char *filename, Image im)
{
  float immax, immin;
  void *data;
  FILE *fp;
  fp = fopen(filename, "w");
  if (!fp) {
    fprintf(stderr, "WriteImage: unable to open '%s'\n", filename);
    MPI_Abort(MPI_COMM_WORLD, 1);
    return;
  }

  fprintf(fp, "P5\n");
  fprintf(fp, "%d %d\n", im->NX, im->NY);
  fprintf(fp, "%d\n", im->threshold);
  if (im->threshold > 0 && im->threshold < 256) {
    data = malloc(im->NX*im->NY);
  } else if (im->threshold > 0 && im->threshold < 65536) {
    data = malloc(im->NX*im->NY*2);
  } else {
    fprintf(stderr, "Invalid image maxval, should be in range (0, 65536), is %d\n", im->threshold);
    MPI_Abort(MPI_COMM_WORLD, 1);
    return;
  }
 

  immax = FLT_MIN;
  immin = FLT_MAX;
  for (int i = 0; i < im->NX * im->NY; i++) {
    immax = fmaxf(immax, im->data[i]);
    immin = fminf(immin, im->data[i]);
  }
  if (im->threshold < 256) {
    uint8_t *image = (uint8_t *)data;
    for (int i = 0; i < im->NX * im->NY; i++) {
      float val = im->threshold*((im->data[i] - immin)/(immax - immin)) + 0.5;
      image[i] = (uint8_t)(val);
    }
  } else {
    uint16_t *image = (uint16_t *)data;
    for (int i = 0; i < im->NX * im->NY; i++) {
      float val = im->threshold*((im->data[i] - immin)/(immax - immin)) + 0.5;
      image[i] = (uint16_t)(val);
    }
  }
  size_t nbytes = fwrite(data, im->threshold < 256 ? 1 : 2,
                        (size_t)(im->NX*im->NY),
                        fp);
  if (nbytes != (size_t)(im->NX*im->NY)) {
    fprintf(stderr, "Unable to write data\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
    return;
  }

  free(data);
  fclose(fp);
  return;
}
