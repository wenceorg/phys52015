#ifndef _CHECK_H
#define _CHECK_H
#include "vec.h"
#include "mat.h"
#include "utils.h"

int CheckMatMult(MPI_Comm, const UserOptions);
int CheckMatMatMult(MPI_Comm, const UserOptions);

#endif
