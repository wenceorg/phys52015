#ifndef _BENCH_H
#define _BENCH_H

#include "vec.h"
#include "mat.h"

int BenchMatMult(Mat, Vec, Vec, const char *);
int BenchMatMatMult(Mat, Mat, Mat, MatMultType, const char *);

#endif
