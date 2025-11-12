#include "light.h"
#include "display.h"

light_t light = {.direction = {0, 0, 1}};

argb_t light_apply_intensity(argb_t original_color, float percentage_factor) {
    // TODO: assert percentage factor 0-1

    argb_t a = (original_color & 0xFF000000);
    argb_t r = ((original_color >> 16) & 0xFF) * percentage_factor;
    argb_t g = ((original_color >> 8) & 0xFF) * percentage_factor;
    argb_t b = (original_color & 0xFF) * percentage_factor;

    return a | (r << 16) | (g << 8) | b;
}