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

  double dotab;
  double *dotlocal = NULL;
  double start, end;
#pragma omp parallel default(none) shared(a, b, N, dotab, dotlocal, start, end)
  {
#pragma omp single
    {
      dotab = 0;
      start = omp_get_wtime();
    }
#pragma omp for schedule(static) reduction(+:dotab)
    for (size_t i = 0; i < N; i++) {
      dotab += a[i]*b[i];
    }
#pragma omp single
    {
      end = omp_get_wtime();
      printf("Reduction clause: %g\n", end - start);
    }
    int tid = omp_get_thread_num();
#pragma omp single
    {
      dotab = 0;
      dotlocal = calloc(omp_get_num_threads(), sizeof(*dotlocal));
      start = omp_get_wtime();
    }
#pragma omp for schedule(static)
    for (size_t i = 0; i < N; i++) {
      dotlocal[tid] += a[i]*b[i];
    }
#pragma omp single
    {
      for (int i = 0; i < omp_get_num_threads(); i++) {
        dotab += dotlocal[i];
      }
      free(dotlocal);
      end = omp_get_wtime();
      printf("By hand: %g\n", end - start);
    }

    double dotpartial = 0;
    #pragma omp single
    {
      dotab = 0;
      start = omp_get_wtime();
    }
#pragma omp for schedule(static) nowait
    for (size_t i = 0; i < N; i++) {
      dotpartial += a[i]*b[i];
    }
    #pragma omp critical
    {
      dotab += dotpartial;
    }
    #pragma omp barrier
    #pragma omp single
    {
      end = omp_get_wtime();
      printf("Critical section: %g\n", end - start);
    }
    dotpartial = 0;
    #pragma omp single
    {
      dotab = 0;
      start = omp_get_wtime();
    }
#pragma omp for schedule(static) nowait
    for (size_t i = 0; i < N; i++) {
      dotpartial += a[i]*b[i];
    }
    #pragma omp atomic
      dotab += dotpartial;
    #pragma omp barrier
    #pragma omp single
    {
      end = omp_get_wtime();
      printf("Atomic update: %g\n", end - start);
    }
  }
  free(a);
  free(b);
  return 0;
}
