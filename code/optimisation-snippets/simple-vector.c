void foo(double *x, int N)
{
  int i;
  for (i = 0 ; i < N; i++) {
    x[i] = x[i] + 2*x[i];
  }
}
