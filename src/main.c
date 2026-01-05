#include "../lib/array.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define N_POINTS 729

// stores the actual triangles in 2D to render
triangle_t *triangles_to_render = NULL;

vec3_t camera_position = {0, 0, 0};

mat4_t proj_matrix;

#define COLOR_BLACK 0xFF000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE 0xFF0000FF
#define COLOR_YELLOW 0xFFFFFF00
#define COLOR_GREEN 0xFF00FF00

bool is_running = false;
int previous_frame_time = 0;

void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:

        if (event.key.keysym.sym == SDLK_ESCAPE)
            is_running = false;

        if (event.key.keysym.sym == SDLK_1) {
            render_method_toggle(&render_method);

            switch (render_method) {
            case RENDER_WIRE:
                printf("Render method: wireframe\n");
                break;
            case RENDER_WIRE_VERTEX:
                printf("Render method: wireframe and vertices\n");
                break;
            case RENDER_FILL_TRIANGLE:
                printf("Render method: filled triangles\n");
                break;
            case RENDER_FILL_TRIANGLE_WIRE:
                printf("Render method: filled triangles and "
                       "wireframe\n");
                break;
            case RENDER_TEXTURED:
                printf("Render method: filled triangles and "
                       "wireframe\n");
                break;
            case RENDER_TEXTURED_WIRE:
                printf("Render method: filled triangles and "
                       "wireframe\n");
                break;
            }
            break;
        }

        if (event.key.keysym.sym == SDLK_2) {
            cull_method_toggle(&cull_method);

            switch (cull_method) {
            case CULL_NONE:
                printf("Backface culling disabled\n");
                break;
            case CULL_BACKFACE:
                printf("Backface culling enabled\n");
                break;
            }
        }
    }
}

void setup(void) {
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

    color_buffer =
        (argb_t *)malloc(sizeof(argb_t) * window_width * window_height);

    // TODO: check malloc

    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             window_width, window_height);

    // TODO: make deg conversion math util
    float fov = M_PI / 3.0; // 60 deg
    float aspect = window_height / (float)window_width;
    float znear = 0.1;
    float zfar = 100.0;

    proj_matrix = mat4_perspective(fov, aspect, znear, zfar);

    load_obj_file_data("./assets/cube.obj");
    load_png_texture_data("./assets/cube.png");
}

/*vec2_t project(vec3_t point) {*/
/*  vec2_t projected = {*/
/*      (fov_factor * point.x) / point.z,*/
/*      (fov_factor * point.y) / point.z,*/
/*  };*/
/**/
/*  return projected;*/
/*}*/

void update(void) {

    int time_to_wait =
        FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    triangles_to_render = NULL;

    mesh.rotation.x += 0.02;
    mesh.rotation.y += 0.03;
    mesh.rotation.z += 0.05;

    /*mesh.scale.x += 0.002;*/
    /*mesh.scale.y += 0.001;*/

    /*mesh.translation.x += 0.01;*/
    mesh.translation.z = 5.0;

    mat4_t scale_matrix = mat4_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_translation(
        mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_x = mat4_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_rotation_z(mesh.rotation.z);

    // loop all triangle faces
    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];

        // loop all 3 vertices of the face and apply transformations
        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Create world matrix combining scale, rotation and
            // translation.
            mat4_t world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // Transform vector by world matrix.
            transformed_vertex =
                mat4_mul_vec4(world_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        // TODO: check backface culling
        vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
        vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
        vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);

        vec3_t vector_ab = vec3_sub(vector_b, vector_a);
        vec3_t vector_ac = vec3_sub(vector_c, vector_a);
        vec3_normalize(&vector_ab);
        vec3_normalize(&vector_ac);

        vec3_t normal = vec3_cross(vector_ab, vector_ac);
        vec3_normalize(&normal);

        vec3_t camera_ray = vec3_sub(camera_position, vector_a);

        // calc how aligned the camera ray is with the face normal
        float dot_normal_camera = vec3_dot(normal, camera_ray);

        // bypass the triangles that are looking away from the
        // camera
        if (cull_method == CULL_BACKFACE) {
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        vec4_t projected_points[3];

        // loop all 3 vertices of the face to perform projection
        for (int j = 0; j < 3; j++) {
            projected_points[j] =
                mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

            // Invert Y for some reason
            projected_points[j].y *= -1;

            // Scale into the view
            projected_points[j].x *= (window_width / 2.0);
            projected_points[j].y *= (window_height / 2.0);

            // Translate the projected points to the middle of the
            // screen
            projected_points[j].x += (window_width / 2.0);
            projected_points[j].y += (window_height / 2.0);
        }

        // Calc the avg depth for each face based on the vertices
        // after transformation
        float avg_depth =
            (transformed_vertices[0].z + transformed_vertices[1].z +
             transformed_vertices[2].z) /
            3.0;

        // Calculate the shade intensity based on how aligned the face normal
        // and the light are.
        float light_intensity_factor = -vec3_dot(normal, light.direction);

        argb_t triangle_color =
            light_apply_intensity(mesh_face.color, light_intensity_factor);

        triangle_t projected_triangle = {
            .points = {{
                           projected_points[0].x,
                           projected_points[0].y,
                           projected_points[0].z,
                           projected_points[0].w,
                       },
                       {
                           projected_points[1].x,
                           projected_points[1].y,
                           projected_points[1].z,
                           projected_points[1].w,
                       },
                       {
                           projected_points[2].x,
                           projected_points[2].y,
                           projected_points[2].z,
                           projected_points[2].w,
                       }},
            .texcoords =
                {
                    {mesh_face.a_uv.u, mesh_face.a_uv.v},
                    {mesh_face.b_uv.u, mesh_face.b_uv.v},
                    {mesh_face.c_uv.u, mesh_face.c_uv.v},
                },
            .color = triangle_color,
            .avg_depth = avg_depth};

        // save the projected triangle in the array of triangles to render
        /*triangles_to_render[i] = projected_triangle;*/
        array_push(triangles_to_render, projected_triangle);
    }

    // Sorting the triangles the render by their average depth
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        for (int j = i; j < num_triangles; j++) {
            if (triangles_to_render[i].avg_depth <
                triangles_to_render[j].avg_depth) {
                triangle_t temp = triangles_to_render[i];
                triangles_to_render[i] = triangles_to_render[j];
                triangles_to_render[j] = temp;
            }
        }
    }
}

void render(void) {
    draw_grid(0xFF555555);

    // loop all projected triangles and render them
    int num_triangles = array_length(triangles_to_render);

    for (int i = 0; i < num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        float x0 = triangle.points[0].x;
        float y0 = triangle.points[0].y;
        float z0 = triangle.points[0].z;
        float w0 = triangle.points[0].w;
        float x1 = triangle.points[1].x;
        float y1 = triangle.points[1].y;
        float z1 = triangle.points[1].z;
        float w1 = triangle.points[1].w;
        float x2 = triangle.points[2].x;
        float y2 = triangle.points[2].y;
        float z2 = triangle.points[2].z;
        float w2 = triangle.points[2].w;

        float u0 = triangle.texcoords[0].u;
        float v0 = triangle.texcoords[0].v;
        float u1 = triangle.texcoords[1].u;
        float v1 = triangle.texcoords[1].v;
        float u2 = triangle.texcoords[2].u;
        float v2 = triangle.texcoords[2].v;

        if (render_method == RENDER_FILL_TRIANGLE ||
            render_method == RENDER_FILL_TRIANGLE_WIRE)
            draw_filled_triangle(x0, y0, x1, y1, x2, y2, triangle.color);

        if (render_method == RENDER_TEXTURED ||
            render_method == RENDER_TEXTURED_WIRE) {
            draw_textured_triangle(x0, y0, z0, w0, u0, v0, x1, y1, z1, w1, u1,
                                   v1, x2, y2, z2, w2, u2, v2, mesh_texture);
        }

        if (render_method == RENDER_WIRE ||
            render_method == RENDER_WIRE_VERTEX ||
            render_method == RENDER_FILL_TRIANGLE_WIRE ||
            render_method == RENDER_TEXTURED_WIRE)
            // draw_filled_triangle(x0, y0, x1, y1, x2, y2, COLOR_BLUE);
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                          triangle.points[1].x, triangle.points[1].y,
                          triangle.points[2].x, triangle.points[2].y,
                          COLOR_BLUE);

        if (render_method == RENDER_WIRE_VERTEX) {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6,
                      COLOR_BLUE);
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6,
                      COLOR_BLUE);
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6,
                      COLOR_BLUE);
        }
    }

    // clear the array of tris to render every frame loop
    array_free(triangles_to_render);

    // render to sdl buffer
    render_color_buffer();

    // draw for next frame
    clear_color_buffer(COLOR_BLACK);

    SDL_RenderPresent(renderer);
}

void free_resources() {
    free(color_buffer);
    free(z_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
    upng_free(png_texture);
}

int main(void) {
    is_running = init_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    free_resources();

    return 0;
}
