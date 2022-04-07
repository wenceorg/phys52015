void foo(double *x, int N)
{
  int i;
  for (i = 1 ; i < N; i++) {
    x[i] = x[i-1] + x[i];
  }
}
