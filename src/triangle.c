#include "triangle.h"
#include "display.h"

#define SWAP(type, a, b)                                                       \
    do {                                                                       \
        type _tmp = (a);                                                       \
        (a) = (b);                                                             \
        (b) = _tmp;                                                            \
    } while (0)

// TODO: make fill functions safe

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
        SWAP(int, y0, y1);
        SWAP(int, x0, x1);
    }

    if (y1 > y2) {
        SWAP(int, y1, y2);
        SWAP(int, x1, x2);
    }

    if (y0 > y1) {
        SWAP(int, y0, y1);
        SWAP(int, x0, x1);
    }

    if (y1 == y2) {
        // there is no bottom half
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    } else if (y0 == y1) {
        // there is no top half
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    } else {
        int my = y1;
        int mx = (float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0) + x0;

        fill_flat_bottom_triangle(x0, y0, x1, y1, mx, my, color);
        fill_flat_top_triangle(x1, y1, mx, my, x2, y2, color);
    }
}

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
    vec2_t ac = vec2_sub(c, a);
    vec2_t ab = vec2_sub(b, a);
    vec2_t ap = vec2_sub(p, a);
    vec2_t pc = vec2_sub(c, p);
    vec2_t pb = vec2_sub(b, p);

    // TODO: cross product, to lin_algebra.h ??
    float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x);

    float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;
    float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;

    float gamma = 1 - alpha - beta;

    vec3_t weights = {alpha, beta, gamma};

    return weights;
}

// all of that necessary? cant i do the interpolation outside?
void draw_texel(int x, int y, uint32_t *texture, vec4_t point_a, vec4_t point_b,
                vec4_t point_c, tex2_t a_uv, tex2_t b_uv, tex2_t c_uv) {
    vec2_t point_p = {x, y};
    vec2_t a = {.x = point_a.x, .y = point_a.y};
    vec2_t b = {.x = point_b.x, .y = point_b.y};
    vec2_t c = {.x = point_c.x, .y = point_c.y};

    vec3_t weights = barycentric_weights(a, b, c, point_p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    if (alpha < 0.0f || beta < 0.0f || gamma < 0.0f)
        return;

    float interpolated_u = (a_uv.u / point_a.w) * alpha +
                           (b_uv.u / point_b.w) * beta +
                           (c_uv.u / point_c.w) * gamma;

    float interpolated_v = (a_uv.v / point_a.w) * alpha +
                           (b_uv.v / point_b.w) * beta +
                           (c_uv.v / point_c.w) * gamma;

    float interpolated_reciprocal_w = (1 / point_a.w) * alpha +
                                      (1 / point_b.w) * beta +
                                      (1 / point_c.w) * gamma;

    interpolated_u /= interpolated_reciprocal_w;

    interpolated_v /= interpolated_reciprocal_w;

    int tex_x = abs((int)(interpolated_u * texture_width));
    int tex_y = abs((int)(interpolated_v * texture_height));

    draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);
    // draw_pixel(x, y, texture[300]);
}

void int_swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void float_swap(float *a, float *b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

void draw_textured_triangle(int x0, int y0, float z0, float w0, float u0,
                            float v0, int x1, int y1, float z1, float w1,
                            float u1, float v1, int x2, int y2, float z2,
                            float w2, float u2, float v2, uint32_t *texture) {
    // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        SWAP(int, y0, y1);
        SWAP(int, x0, x1);
        SWAP(float, z0, z1);
        SWAP(float, w0, w1);
        SWAP(float, u0, u1);
        SWAP(float, v0, v1);
    }

    if (y1 > y2) {
        SWAP(int, y1, y2);
        SWAP(int, x1, x2);
        SWAP(float, z1, z2);
        SWAP(float, w1, w2);
        SWAP(float, u1, u2);
        SWAP(float, v1, v2);
    }

    if (y0 > y1) {
        SWAP(int, y0, y1);
        SWAP(int, x0, x1);
        SWAP(float, z0, z1);
        SWAP(float, w0, w1);
        SWAP(float, u0, u1);
        SWAP(float, v0, v1);
    }

    // Create vector points and texture coords after we sort the vertices
    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};
    tex2_t a_uv = {u0, v0};
    tex2_t b_uv = {u1, v1};
    tex2_t c_uv = {u2, v2};

    // TOP PART

    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0)
        inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0)
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            float x_start = x1 + (y - y1) * inv_slope_1;
            float x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                SWAP(int, x_start, x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv,
                           c_uv);
            }
        }
    }

    // BOTTOM PART

    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0)
        inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0)
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            float x_start = x1 + (y - y1) * inv_slope_1;
            float x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                SWAP(int, x_start, x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv,
                           c_uv);
            }
        }
    }
}
