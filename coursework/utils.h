#ifndef _UTILS_H
#define _UTILS_H
#include <mpi.h>
#include <stdio.h>
#include "vec.h"
#define CHKERR(ierr) do { if (ierr) { fprintf(stderr, "MPI failed with return code %d\n", ierr); return MPI_Abort(MPI_COMM_WORLD, ierr); } } while (0)

#endif
