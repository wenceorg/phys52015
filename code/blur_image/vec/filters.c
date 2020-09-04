#include "math.h"
#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*blur filter: we use a square window function with equal weights

                 |1|1|1|
 Output = 1/9 *  |1|1|1|  * Image
                 |1|1|1|

 */

struct Image blur_mean(struct Image input, int n) {
  struct Image output;
  int i, j;
  int dimx, dimy;
  clock_t start, end;

  printf(GRN "Applying mean blur filter... \n " RESET);
  fflush(stdout);

  /* We define the output Image */
  //-----------------------------//
  dimx = input.dimx;
  dimy = input.dimy;

  output.r = (float *)malloc(sizeof(float) * dimx * dimy);
  output.g = (float *)malloc(sizeof(float) * dimx * dimy);
  output.b = (float *)malloc(sizeof(float) * dimx * dimy);

  output.dimx = dimx;
  output.dimy = dimy;

  float npixels = pow(2 * n + 1.0, 2.0);

  // initialize the output
  for (i = 0; i < dimx * dimy; i++) {
    output.r[i] = 0.0;
    output.g[i] = 0.0;
    output.b[i] = 0.0;
  }
  //-----------------------------//

  // Automatic loop//
  //-----------------------------//
  start = clock();
  for (int id = 0; id < dimx * dimy; id++) {

    i = input.xcoord[id];
    j = input.ycoord[id];

    for (int k = -n; k <= n; k++) {
      int idx = i + k;
      if (idx < 0)
        idx = i + k + dimx;
      if (idx >= dimx)
        idx = i + k - dimx;

      for (int l = -n; l <= n; l++) {
        int idy = j + l;

        if (idy < 0)
          idy = j + l + dimy;
        if (idy >= dimy)
          idy = j + l - dimy;

        int id_out = idx + dimx * idy;

        output.r[id] += input.r[id_out] * 1.0 / npixels;
        output.g[id] += input.g[id_out] * 1.0 / npixels;
        output.b[id] += input.b[id_out] * 1.0 / npixels;
      }
    }
  }

  //---------------------------//
  end = clock();

  printf(MAG "Automatic loop took:%6f\n" RESET,
         ((double)end - start) / CLOCKS_PER_SEC);

  printf(BLU "Done \n" RESET);

  return output;
}
