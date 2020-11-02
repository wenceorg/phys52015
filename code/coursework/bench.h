#ifndef _BENCH_H
#define _BENCH_H

#include "vec.h"
#include "mat.h"
#include "utils.h"

int BenchMatMult(MPI_Comm, const UserOptions);
int BenchMatMatMult(MPI_Comm, const UserOptions);

#endif
