#include "mesh.h"

// TODO: implement mesh.h functions

vec3_t mesh_vertices[N_MESH_VERTICES] = {
    {.x = -1, .y = -1, .z = -1},
    {-1, 1, -1},
    {1, 1, -1},
    {1, -1, -1},
    {1, 1, 1},
    {1, -1, 1},
    {-1, 1, 1},
    {-1, -1, 1},
};

face_t mesh_faces[N_MESH_FACES] = {
    {.a = 1, .b = 2, .c = 3},
    {1, 3, 4},
    {4, 3, 5},
    {4, 5, 6},
    {6, 5, 7},
    {6, 7, 8},
    {8, 7, 2},
    {8, 2, 1},
    {2, 7, 5},
    {2, 5, 3},
    {6, 8, 1},
    {6, 1, 4},
};
