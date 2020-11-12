#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <limits.h>

int main(int argc, char **argv)
{
  const size_t N = argc > 1 ? strtoul(argv[1], NULL, 10) : 1024u;
  double *a = malloc(N * sizeof(*a));
  double *b = malloc(N * sizeof(*b));

  printf("Computing dot product with %lu entries\n", N);
  /* Intialise with some values */
#pragma omp parallel for default(none) schedule(static) shared(a, b, N)  
  for (size_t i = 0; i < N; i++) {
    a[i] = i+1;
    b[i] = (-1)*(i%2) * i;
  }

  double dotabparallel = 0;
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
    for (size_t i = 0; i < N; i++) {
    }
  }
  printf("Parallel a.b = %.4g; took %.4g seconds\n", dotabparallel, omp_get_wtime() - start);
  free(a);
  free(b);
  return 0;
}
