#ifndef _BENCH_H
#define _BENCH_H

#include "vec.h"
#include "mat.h"

int BenchMatMult(Mat, Vec, Vec);
int BenchMatMatMult(Mat, Mat, Mat, MatMultType);

#endif
