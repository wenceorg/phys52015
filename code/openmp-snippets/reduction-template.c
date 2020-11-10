#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char **argv)
{
  const int N = argc > 1 ? atoi(argv[1]) : 1024;
  int *a = malloc(N * sizeof(*a));
  int *b = malloc(N * sizeof(*b));
  /* Intialise with some values */
#pragma omp parallel for default(none) shared(a, b, N)  
  for (int i = 0; i < N; i++) {
    a[i] = i+1;
    b[i] = (-1)*(i%2) * i;
  }
  /* Check */
  int dotabserial = 0;
  double start = omp_get_wtime();
  for (int i = 0; i < N; i++) {
    dotabserial += a[i]*b[i];
  }
  printf("  Serial a.b = %d; took %.4g seconds\n", dotabserial, omp_get_wtime() - start);

  int dotabparallel = 0;
  start = omp_get_wtime();
#pragma omp parallel default(none) shared(a, b, N, dotabparallel)
  {
    /* Implement a parallel dot product using
     *
     * 1. The approach of reduction-hand.c
     * 2. the reduction clause
     * 3. critical sections to protect the shared updates
     * 4. atomics to protect the shared updates.
     */
  }
  printf("Parallel a.b = %d; took %.4g seconds\n", dotabparallel, omp_get_wtime() - start);
  free(a);
  free(b);
  return 0;
}
