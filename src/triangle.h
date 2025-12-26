#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "display.h"
#include "texture.h"
#include "vector.h"

typedef struct {
    int a;
    int b;
    int c;
    tex2_t a_uv;
    tex2_t b_uv;
    tex2_t c_uv;
    argb_t color;
} face_t;

typedef struct {
    vec4_t points[3];
    tex2_t texcoords[3];
    argb_t color;
    float avg_depth;
} triangle_t;

typedef vec4_t point_t;

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                          argb_t color);

// void triangle_draw_textured(triangle_t *triangle, texture_t *texture);

void draw_textured_triangle(int x0, int y0, float z0, float w0, float u0,
                            float v0, int x1, int y1, float z1, float w1,
                            float u1, float v1, int x2, int y2, float z2,
                            float w2, float u2, float v2, uint32_t *texture);

void draw_texel(int x, int y, uint32_t *texture, vec4_t point_a, vec4_t point_b,
                vec4_t point_c, tex2_t a_uv, tex2_t b_uv, tex2_t c_uv);

#endif
