#include <omp.h>
#include <stdio.h>
#include <unistd.h>
void foo(double *a, int N)
{
  int i;
#pragma omp parallel shared(a, N) private(i)
  {
    int j; /* This variable is local to the block (and hence private) */

    /* Each thread has its own copy of i. */
    i = omp_get_thread_num();
    j = i;
    if (i%2 == 0) {
      /* Fake "delay" of some threads. */
      usleep(10);
    }
    /* All threads write to the same a and read the same N. */
    if (j < N) a[j] = i;
  }
}

int main(int argc, char **argv)
{
  int N = 32;
  double a[32];

  for (int i = 0; i < N; i++) {
    a[i] = -1;
  }

  foo(a, N);

  for (int i = 0; i < N; i++) {
    if (a[i] > -1) {
      printf("a[%2d] = %g\n", i, a[i]);
    }
  }
}
