#include <stdio.h>
#include <omp.h>

int main(void)
{
  int nthread = omp_get_max_threads();
  int thread;
#pragma omp parallel private(thread) shared(nthread)
  {
    thread = omp_get_thread_num();
    printf("Hello, World! I am thread %d of %d\n", thread, nthread);
  }
  return 0;
}
