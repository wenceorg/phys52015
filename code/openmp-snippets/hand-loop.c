#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
static inline int min(int a, int b)
{
  return a < b ? a : b;
}

int main(void)
{
  const int N = 16;
  int a[N];
  for (int i = 0; i < N; i++) {
    /* Sentinel for unhandled value */
    a[i] = -1;
  }
#pragma omp parallel default(none) shared(N, a)
  {
    int tid = omp_get_thread_num();
    int nthread = omp_get_num_threads();

    int chunk = N / nthread + ((N % nthread) > tid);
    int start = tid * (N / nthread) + min(tid, N % nthread);
    for (int i = start; i < start + chunk; i++) {
      a[i] = tid;
    }
  }
  for (int i = 0; i < N; i++) {
    printf("a[%2d] = %2d\n", i, a[i]);
  }
  return 0;
}
