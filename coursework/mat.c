#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <cblas.h>
#include "vec.h"
#include "mat.h"
#include "utils.h"

static int MatProcessGrid_Private(MPI_Comm comm, int *np)
{
  int ierr;
  int size;
  ierr = MPI_Comm_size(comm, &size);CHKERR(ierr);

  *np = (int)sqrt(size);
  if (size != (*np) * (*np)) {
    fprintf(stderr, "MatCreate: need square process grid for matrix distribution.\n");
    return MPI_Abort(comm, MPI_ERR_UNKNOWN);
  }
  return 0;
}

/* Create a square matrix.
 * - comm: communicator
 * - N: Global number of rows.
 * - mat: pointer to output matrix structure.
 */
int MatCreate(MPI_Comm comm, int N, Mat *mat)
{
  int ierr;
  int rank;
  Mat a = calloc(1, sizeof(struct _p_Mat));

  if (!a) {
    fprintf(stderr, "Unable to allocate space for matrix\n");
    return MPI_Abort(comm, MPI_ERR_NO_MEM);
  }
  ierr = MatProcessGrid_Private(comm, &a->np);CHKERR(ierr);
  a->n = N / a->np;
  a->N = N;
  if (a->n * a->np != N) {
    ierr = MPI_Comm_rank(comm, &rank);CHKERR(ierr);
    fprintf(stderr, "[%d] MatCreate: need equal number of rows on each process.\n", rank);
    fprintf(stderr, "[%d] Global rows %d not evenly divisible by %d.\n", rank, N, a->np);
    return MPI_Abort(comm, MPI_ERR_ARG);
  }
  a->comm = comm;
  a->data = calloc((size_t)(a->n*a->n), sizeof(*a->data));
  if (!a->data) {
    fprintf(stderr, "Unable to allocate space for matrix\n");
    return MPI_Abort(comm, MPI_ERR_NO_MEM);
  }
  *mat = a;
  return 0;
}

/* Destroy a matrix
 * - mat: pointer to matrix (may be NULL)
 */
int MatDestroy(Mat *mat)
{
  if (!*mat) return 0;
  free((*mat)->data);
  free(*mat);
  *mat = NULL;
  return 0;
}


/* View a matrix to a file.
 *
 * - mat: Matrix to view
 * - file: File to output to (may be NULL for standard output)
 */
int MatView(Mat mat, FILE *file)
{
  int rank;
  int ierr;
  int *recvcounts = NULL;
  int *displacements = NULL;
  double *gmat = NULL;
  MPI_Datatype roottype = MPI_DATATYPE_NULL;
  ierr = MPI_Comm_rank(mat->comm, &rank);CHKERR(ierr);
  if (!file) {
    file = stdout;
  }
  if (!rank) {
    MPI_Datatype vector;
    int size, np;

    ierr = MPI_Comm_size(mat->comm, &size);CHKERR(ierr);
    gmat = malloc(sizeof(*gmat)*mat->N*mat->N);
    if (!gmat) {
      fprintf(stderr, "Unable to allocate space for matrix\n");
      return MPI_Abort(MPI_COMM_SELF, MPI_ERR_NO_MEM);
    }
    ierr = MPI_Type_vector(mat->N, mat->n, mat->N, MPI_DOUBLE, &vector);CHKERR(ierr);
    ierr = MPI_Type_create_resized(vector, 0, sizeof(double), &roottype);CHKERR(ierr);
    ierr = MPI_Type_free(&vector);CHKERR(ierr);
    ierr = MPI_Type_commit(&roottype);CHKERR(ierr);
    recvcounts = malloc(size*sizeof(*recvcounts));
    if (!recvcounts) {
      fprintf(stderr, "Unable to allocate space\n");
      return MPI_Abort(MPI_COMM_SELF, MPI_ERR_NO_MEM);
    }
    for (int i = 0; i < size; i++) {
      recvcounts[i] = 1;
    }
    displacements = malloc(size*sizeof(*displacements));
    if (!displacements) {
      fprintf(stderr, "Unable to allocate space\n");
      return MPI_Abort(MPI_COMM_SELF, MPI_ERR_NO_MEM);
    }
    np = mat->np;
    for (int i = 0; i < np; i++) {
      for (int j = 0; j < np; j++) {
        displacements[i*np + j] = i*mat->n*mat->N + j*mat->n;
      }
    }
  }
  ierr = MPI_Gatherv(mat->data, mat->n*mat->n, MPI_DOUBLE,
                     gmat, recvcounts, displacements, roottype, 0,
                     mat->comm);CHKERR(ierr);
  if (!rank) {
    int size;
    ierr = MPI_Comm_size(mat->comm, &size);CHKERR(ierr);
    fprintf(file, "Matrix distributed over %d processes\n", size);
    fprintf(file, "  Global size: %d x %d\n", mat->N, mat->N);
    fprintf(file, "  Local size: %d x %d\n", mat->n, mat->n);
    fprintf(file, "  Entries:\n");
    for (int i = 0; i < mat->N; i++) {
      for (int j = 0; j < mat->N; j++) {
        fprintf(file, "%g ", gmat[i*mat->N + j]);
      }
      fprintf(file, "\n");
    }
    fprintf(file, "\n");
    ierr = MPI_Type_free(&roottype);CHKERR(ierr);
  }
  free(recvcounts);
  free(displacements);
  free(gmat);
  return 0;
}

/* Do a local part of y <- Ax
 * For square matrix, and compatible sized vectors
 * - n: number of rows and columns.
 * - a: matrix entries, in row-major form.
 * - x: vector entries (input)
 * - y: output vector.
 */
int MatMultLocal(int n, const double *a, const double *x, double *y)
{
  cblas_dgemv(CblasRowMajor, CblasNoTrans,
              n, n,
              1, a, n,
              x, 1,
              0,
              y, 1);
  return 0;
}

/* Do local part of C <- AB + C
 * For square matrices, all of same size.
 * - n: number of rows and columns
 * - a: matrix entries for a (row major)
 * - b: matrix entries for b (row major)
 * - c: output matrix entries (row major)
 */
int MatMatMultLocal(int n, const double *a,
                           const double *b,
                           double *c)
{
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
              n, n, n,
              1, a, n, b, n,
              1, c, n);
  return 0;
}


/* C <- AB + C
 *
 * - A: input matrix
 * - B: input matrix
 * - C: output matrix
 * - algorithm: Whether to use Cannon's algorithm or SUMMA.
 */
int MatMatMult(Mat A, Mat B, Mat C, MatMultType algorithm)
{
  if (A->n != B->n || A->n != C->n || A->N != B->N || A->N != B->N) {
    fprintf(stderr, "Mismatching matrix sizes in matrix multiplication\n");
    return MPI_Abort(A->comm, MPI_ERR_ARG);
  }
  switch (algorithm) {
  case MAT_MULT_SUMMA:
    return MatMatMultSumma(A, B, C);
  case MAT_MULT_CANNON:
    return MatMatMultCannon(A, B, C);
  default:
    fprintf(stderr, "Unknown matrix multiplication algorithm\n");
    return MPI_Abort(A->comm, MPI_ERR_ARG);
  }
}
