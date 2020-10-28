#define MR 4
#define NR 8

void micro_kernel(int kc,
                  const double * restrict A,
                  const double * restrict B,
                  double * restrict AB)
{
  int i, j, l;
  for (l = 0; l < kc; ++l) {
#pragma unroll(8)
    for (j = 0; j < NR; ++j)
#pragma omp simd
      for (i = 0; i < MR; ++i)
        AB[i + j*MR] += A[i] * B[j];
    A += MR;
    B += NR;
  }
}
