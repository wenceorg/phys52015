---
title: "OpenMP: synchronisation"
weight: 6
---

# Some stuff


In [`openmp-snippets/reduction-template.c`]({{< code-ref
"openmp-snippets/reduction.template.c" >}}) is some code that times
how long it takes to run the reduction.

You can select the length of the vector the compute the dot product of
by passing a size on the commandline. For example, after compiling with

```
$ icc -qopenmp -o reduction-template reduction-template.c
```

You can run, for example, with
```
$ OMP_NUM_THREADS=2 ./reduction-template 1000000
```

The parallel reduction is not implemented. Try implementing the
reduction using the four different approaches suggested in the code.

For a large vector (100
