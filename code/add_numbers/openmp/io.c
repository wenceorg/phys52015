// This file is part of the HPC workshop 2019 at Durham University
// Author: Christian Arnold

#define _XOPEN_SOURCE

#include "proto.h"
#include <stdio.h>
#include <stdlib.h>

/* This function reads the numbers from the file. */
void read_numbers(char *filename, int *n_numbers, float **numbers) {

  FILE *fp;
  if (!(fp = fopen(filename, "rb"))) {
    printf("ERROR: Cannot read file %s \n", filename);
    exit(1);
  }

  printf("Reading number file: %s ... \n", filename);

  if (!(fread(n_numbers, sizeof(n_numbers), 1, fp) == 1)) {
    printf("ERROR: Cannot read n_numbers\n");
    exit(1);
  }

  printf("Found %i numbers in file.\n", *n_numbers);

  if ((*numbers = (float *)malloc(*n_numbers * sizeof(float))) == NULL) {
    printf("ERROR: Cannot allocate memory for n_numbers\n");
    exit(1);
  }

  for (int i = 0; i < *n_numbers; i++) {
    if (fread(&(*numbers)[i], sizeof((*numbers)[i]), 1, fp) != 1) {
      printf("ERROR: Cannot read numbers\n");
      exit(1);
    }
  }
  fclose(fp);
}

/*
 * This function generates random numbers and writes them to file.
 * (You do not need to worry about this, it has already been done for you.)
 */
void create_and_write_numbers(char *filename, int n_numbers) {
  float *numbers;

  printf("Creating %i random numbers\n", n_numbers);
  /* allocate memoty for numbers */
  if ((numbers = (float *)malloc(n_numbers * sizeof(float))) == NULL) {
    printf("ERROR: Cannot allocate memory for n_numbers\n");
    exit(1);
  }

  printf("Filling array ...\n");
  /* generate some random numbers */
  for (int i = 0; i < n_numbers; i++) {
    numbers[i] = (float)(drand48() * 2. -
                         1.); /* random numbers in the interval (-1, 1) */
  }

  printf("opening file ...\n");

  FILE *fp;
  if (!(fp = fopen(filename, "wb"))) {
    printf("ERROR: Cannot open file %s \n", filename);
    exit(1);
  }

  printf("Writing number file: %s ... \n", filename);

  if (fwrite(&n_numbers, sizeof(&n_numbers), 1, fp) != 1) {
    printf("ERROR: Cannot write n_numbers\n");
    exit(1);
  }

  for (int i = 0; i < n_numbers; i++) {
    if (fwrite(&numbers[i], sizeof(numbers[i]), 1, fp) != 1) {
      printf("ERROR: Cannot write numbers\n");
      exit(1);
    }
  }

  fclose(fp);
  free(numbers);
  printf("Done.\n");
}
