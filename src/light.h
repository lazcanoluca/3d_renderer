#ifndef LIGHT_H
#define LIGHT_H

#include "display.h"
#include "vector.h"

typedef struct {
    vec3_t direction;
} light_t;

// TODO: remove these globals stuff, if i want a light
// i should define it in a scene object or something
extern light_t light;

argb_t light_apply_intensity(argb_t original_color, float percentage_factor);

#endif
