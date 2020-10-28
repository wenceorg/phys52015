#ifndef _VEC_H
#define _VEC_H
#include <stdio.h>
#include <mpi.h>

struct _p_Vec {
  MPI_Comm comm;                /* communicator */
  int n, N;                     /* local and global size */
  int np;                       /* number of processes */
  double *data;                 /* vector entries */
};

typedef struct _p_Vec *Vec;


int VecCreate(MPI_Comm, int, Vec *);
int VecDestroy(Vec *);
int VecView(Vec, FILE *);

#endif
