#include <stdlib.h>
#include "image.h"

void CreateImage(Image *image)
{
  Image im;
  im = calloc(1, sizeof(*im));
  *image = im;
}

void SetSizes(Image image, int NX, int NY)
{
  image->NX = NX;
  image->NY = NY;
  image->data = calloc(NX * NY, sizeof(*image->data));
  image->top_boundary = calloc(NX, sizeof(*image->top_boundary));
  image->bottom_boundary = calloc(NX, sizeof(*image->bottom_boundary));
}

void SetThreshold(Image image, int threshold)
{
  image->threshold = threshold;
}

void CopyImage(Image in, Image *out)
{
  CreateImage(out);
  SetSizes(*out, in->NX, in->NY);
  SetThreshold(*out, in->threshold);
  for (int i = 0; i < in->NX * in->NY; i++) {
    (*out)->data[i] = in->data[i];
  }
  for (int i = 0; i < in->NX; i++) {
    (*out)->top_boundary[i] = in->top_boundary[i];
    (*out)->bottom_boundary[i] = in->bottom_boundary[i];
  }
}

void DestroyImage(Image *image)
{
  if (!*image) return;
  free((*image)->data);
  free((*image)->top_boundary);
  free((*image)->bottom_boundary);
  free(*image);
  *image = NULL;
}

