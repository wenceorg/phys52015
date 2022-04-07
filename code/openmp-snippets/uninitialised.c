#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
int main(void)
{
  int b = 42;
  double *a = malloc(100*sizeof(*a));

#pragma omp parallel default(none) shared(a) private(b)
  {
    a[omp_get_thread_num()] = 2;

    printf("Thread=%d; b=%d\n", omp_get_thread_num(), b);
  }
  free(a);
  return 0;
}
