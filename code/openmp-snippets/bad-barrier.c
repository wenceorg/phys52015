#include <omp.h>
#include <stdio.h>


int main(void)
{
  const int N = 16;
  int a[N];
  for (int i = 0; i < N; i++) {
    a[i] = -1;
  }
#pragma omp parallel default(none) shared(a, N)
  {
    int tid = omp_get_thread_num();
    int nthread = omp_get_num_threads();

    a[tid] = tid + nthread;

    if (tid % 2 == 0) {
      /* deadlock! */
      #pragma omp barrier
    }
    a[tid] = a[(tid + 1)%nthread];
  }
  for (int i = 0; i < N; i++) {
    printf("a[%2d] = %2d\n", i, a[i]);
  }
  return 0;
}
