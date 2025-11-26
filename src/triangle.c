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
void draw_texel(int x, int y, uint32_t *texture, vec2_t point_a, vec2_t point_b,
                vec2_t point_c, float u0, float v0, float u1, float v1,
                float u2, float v2) {
    vec2_t point_p = {x, y};

    vec3_t weights = barycentric_weights(point_a, point_b, point_c, point_p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    float interpolated_u = (u0)*alpha + (u1)*beta + (u2)*gamma;
    float interpolated_v = (v0)*alpha + (v1)*beta + (v2)*gamma;

    int tex_x = abs((int)(interpolated_u * texture_width));
    int tex_y = abs((int)(interpolated_v * texture_height));

    draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);
    // draw_pixel(x, y, texture[300]);
}

void draw_textured_triangle(int x0, int y0, float u0, float v0, int x1, int y1,
                            float u1, float v1, int x2, int y2, float u2,
                            float v2, uint32_t *texture) {

    if (y0 > y1) {
        SWAP(int, y0, y1);
        SWAP(int, x0, x1);
        SWAP(float, u0, u1);
        SWAP(float, v0, v1);
    }

    if (y1 > y2) {
        SWAP(int, y1, y2);
        SWAP(int, x1, x2);
        SWAP(float, u1, u2);
        SWAP(float, v1, v2);
    }

    if (y0 > y1) {
        SWAP(int, y0, y1);
        SWAP(int, x0, x1);
        SWAP(float, u0, u1);
        SWAP(float, v0, v1);
    }

    vec2_t point_a = {x0, y0};
    vec2_t point_b = {x1, y1};
    vec2_t point_c = {x2, y2};

    // TOP PART

    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0)
        inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);

    if (y2 - y0 != 0)
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                SWAP(int, x_start, x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1,
                           v1, u2, v2);
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
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                SWAP(int, x_start, x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1,
                           v1, u2, v2);
            }
        }
    }
}