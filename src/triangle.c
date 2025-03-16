#include "triangle.h"
#include "display.h"

// TODO: implement triangle.h functions

void int_swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                               argb_t color) {
  float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
  float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

  // start x_start and e_end from the top vertex (x0, y0)
  float x_start, x_end;
  x_start = x_end = x0;

  // loop scanlines from top to bottom
  for (int y = y0; y <= y2; y++) {
    draw_line(x_start, y, x_end, y, color);

    x_start += inv_slope_1;
    x_end += inv_slope_2;
  }
}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                            argb_t color) {
  float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
  float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

  // start x_start and e_end from the bottom vertex (x2, y2)
  float x_start, x_end;
  x_start = x_end = x2;

  // loop scanlines from top to bottom
  for (int y = y2; y >= y0; y--) {
    draw_line(x_start, y, x_end, y, color);

    x_start -= inv_slope_1;
    x_end -= inv_slope_2;
  }
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                          argb_t color) {
  // sort line vertices by y coordinate ascending (y0 < y1 < y2)

  if (y0 > y1) {
    int_swap(&y0, &y1);
    int_swap(&x0, &x1);
  }

  if (y1 > y2) {
    int_swap(&y1, &y2);
    int_swap(&x1, &x2);
  }

  if (y0 > y1) {
    int_swap(&y0, &y1);
    int_swap(&x0, &x1);
  }

  int my = y1;
  int mx = (float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0) + x0;

  fill_flat_bottom_triangle(x0, y0, x1, y1, mx, my, color);
  fill_flat_top_triangle(x1, y1, mx, my, x2, y2, color);
}
