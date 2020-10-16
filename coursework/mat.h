#ifndef _MAT_H
#define _MAT_H
#include <stdio.h>
#include <mpi.h>
#include "vec.h"

struct _p_Mat {
  MPI_Comm comm;                /* communicator */
  int n, N;                     /* local and global size */
  int np;                       /* process grid np x np */
  double *data;                 /* matrix entries */
};

typedef struct _p_Mat *Mat;
typedef enum {MAT_MULT_SUMMA, MAT_MULT_CANNON} MatMultType;

int MatCreate(MPI_Comm, int, Mat *);
int MatDestroy(Mat *);
int MatView(Mat, FILE *);
int MatMatMultLocal(int, const double *,
                           const double *, double *);
int MatMatMultSumma(Mat, Mat, Mat);
int MatMatMultCannon(Mat, Mat, Mat);
int MatMatMult(Mat, Mat, Mat, MatMultType);

int MatMultLocal(int, const double *, const double *, double *);
int MatMult(Mat, Vec, Vec);

#endif
