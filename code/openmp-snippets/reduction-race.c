#include <stdio.h>
#include <stdlib.h>

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
#pragma omp parallel default(none) shared(a, b, N, dotabparallel)
  {
#pragma omp for schedule(static)
    for (int i = 0; i < N; i++) {
      dotabparallel += a[i]*b[i];
    }
  }
  printf("Parallel a.b = %d\n", dotabparallel);
  free(a);
  free(b);
  return 0;
}
