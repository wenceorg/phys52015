#include <omp.h>
#include <stdio.h>

int main(void)
{
  int N = 16;
  int a[N];
  for (int i = 0; i < N; i++) {
    /* Sentinel for unhandled value */
    a[i] = -1;
  }
#pragma omp parallel for default(none) shared(a, N)
  for (int i = 0; i < N; i++) {
    a[i] = i;
    i++;
  }

  for (int i = 0; i < N; i++) {
    printf("a[%2d] = %2d\n", i, a[i]);
  }
  return 0;
}
