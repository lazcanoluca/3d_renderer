#include "mesh.h"
#include "../lib/array.h"
#include "vector.h"
#include <stdio.h>
#include <string.h>

mesh_t mesh = {.vertices = NULL,
               .faces = NULL,
               .rotation = {0, 0, 0},
               .scale = {1, 1, 1},
               .translation = {0, 0, 0}};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1},
    {-1, 1, -1},
    {1, 1, -1},
    {1, -1, -1},
    {1, 1, 1},
    {1, -1, 1},
    {-1, 1, 1},
    {-1, -1, 1},
};

face_t cube_faces[N_CUBE_FACES] = {
    {.a = 1,
     .b = 2,
     .c = 3,
     .a_uv = {0, 1},
     .b_uv = {0, 0},
     .c_uv = {1, 0},
     .color = 0xFFFFFFFF},
    {1, 3, 4, {0, 1}, {1, 0}, {1, 1}, 0xFFFFFFFF},
    {4, 3, 5, {0, 1}, {0, 0}, {1, 0}, 0xFFFFFFFF},
    {4, 5, 6, {0, 1}, {1, 0}, {1, 1}, 0xFFFFFFFF},
    {6, 5, 7, {0, 1}, {0, 0}, {1, 0}, 0xFFFFFFFF},
    {6, 7, 8, {0, 1}, {1, 0}, {1, 1}, 0xFFFFFFFF},
    {8, 7, 2, {0, 1}, {0, 0}, {1, 0}, 0xFFFFFFFF},
    {8, 2, 1, {0, 1}, {1, 0}, {1, 1}, 0xFFFFFFFF},
    {2, 7, 5, {0, 1}, {0, 0}, {1, 0}, 0xFFFFFFFF},
    {2, 5, 3, {0, 1}, {1, 0}, {1, 1}, 0xFFFFFFFF},
    {6, 8, 1, {0, 1}, {0, 0}, {1, 0}, 0xFFFFFFFF},
    {6, 1, 4, {0, 1}, {1, 0}, {1, 1}, 0xFFFFFFFF},
};

// face_t cube_faces[N_CUBE_FACES] = {
//     {.a = 1, .b = 2, .c = 3, .color = 0xFF0000FF},
//     {1, 3, 4, 0xFF0000FF},
//     {4, 3, 5, 0xFF00FF00},
//     {4, 5, 6, 0xFF00FF00},
//     {6, 5, 7, 0xFF00FFFF},
//     {6, 7, 8, 0xFF00FFFF},
//     {8, 7, 2, 0xFFFF0000},
//     {8, 2, 1, 0xFFFF0000},
//     {2, 7, 5, 0xFFFF00FF},
//     {2, 5, 3, 0xFFFF00FF},
//     {6, 8, 1, 0xFFFFFF00},
//     {6, 1, 4, 0xFFFFFF00},
// };

void load_cube_mesh_data() {
    for (int i = 0; i < N_CUBE_VERTICES; i++) {
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices, cube_vertex);
    }

    for (int i = 0; i < N_CUBE_FACES; i++) {
        face_t cube_face = cube_faces[i];
        array_push(mesh.faces, cube_face);
    }
}

void load_obj_file_data(char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    char line[1024];

    tex2_t *texcoords = NULL;

    while (fgets(line, 1024, file)) {
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }

        // Texture coordinate information.
        if (strncmp(line, "vt ", 3) == 0) {
            tex2_t texcoord;
            sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
            array_push(texcoords, texcoord);
        }

        // Face information
        if (strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];

            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex_indices[0],
                   &texture_indices[0], &normal_indices[0], &vertex_indices[1],
                   &texture_indices[1], &normal_indices[1], &vertex_indices[2],
                   &texture_indices[2], &normal_indices[2]);

            face_t face = {.a = vertex_indices[0],
                           .b = vertex_indices[1],
                           .c = vertex_indices[2],
                           .a_uv = texcoords[texture_indices[0] - 1],
                           .b_uv = texcoords[texture_indices[1] - 1],
                           .c_uv = texcoords[texture_indices[2] - 1],
                           .color = 0xFFFFFFFF};

            array_push(mesh.faces, face);
        }
    }

    array_free(texcoords);
}
