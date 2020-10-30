#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void)
{
  const int N = 1024;
  int *a = malloc(N * sizeof(*a));
  int *b = malloc(N * sizeof(*b));
  /* Intialise with some values */
  for (int i = 0; i < N; i++) {
    a[i] = i+1;
    b[i] = i*2;
  }
  /* Check */
  int dotabserial = 0;
  for (int i = 0; i < N; i++) {
    dotabserial += a[i]*b[i];
  }

  printf("  Serial a.b = %d\n", dotabserial);

  int dotabparallel = 0;
  int *dotlocal = NULL;
#pragma omp parallel default(none) shared(a, b, N, dotabparallel, dotlocal)
  {
    int tid = omp_get_thread_num();
#pragma omp single
    dotlocal = calloc(omp_get_num_threads(), sizeof(*dotlocal));
    /* Implicit barrier at end of single is required so that dotlocal
       is defined for the loop */
#pragma omp for schedule(static)
    for (int i = 0; i < N; i++) {
      dotlocal[tid] += a[i]*b[i];
    }
    /* Implicit barrier at end of for is required here so that a
       thread finishing early does not update dotabparallel too soon. */
#pragma omp single nowait
    for (int i = 0; i < omp_get_num_threads(); i++) {
      dotabparallel += dotlocal[i];
    }
  }
  printf("Parallel a.b = %d\n", dotabparallel);
  free(dotlocal);
  free(a);
  free(b);
  return 0;
}
