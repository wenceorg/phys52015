#ifndef _IMAGE_H
#define _IMAGE_H
struct _p_Image {
  int NX, NY;
  int threshold;
  float *data;
  float *top_boundary;
  float *bottom_boundary;
};

typedef struct _p_Image * Image;

void CreateImage(Image *);
void DestroyImage(Image *);
void CopyImage(Image, Image *);
void SetSizes(Image, int, int);
void SetThreshold(Image, int);
#endif
