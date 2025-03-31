#include "../lib/array.h"
#include "display.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

#define N_POINTS 729

// stores the actual triangles in 2D to render
triangle_t *triangles_to_render = NULL;

vec3_t camera_position = {0, 0, 0};

float fov_factor = 640;

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
        printf("Render method: filled triangles and wireframe\n");
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

  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);

  /*load_obj_file_data("./assets/f22.obj");*/
  /*load_obj_file_data("./assets/cube.obj");*/
  load_cube_mesh_data();
}

vec2_t project(vec3_t point) {
  vec2_t projected = {
      (fov_factor * point.x) / point.z,
      (fov_factor * point.y) / point.z,
  };

  return projected;
}

void update(void) {

  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();

  triangles_to_render = NULL;

  mesh.rotation.x += 0.01;
  mesh.rotation.y += 0.01;
  mesh.rotation.z += 0.01;

  /*mesh.scale.x += 0.002;*/
  mesh.translation.x += 0.01;
  mesh.translation.z = 5;

  mat4_t scale_matrix = mat4_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
  mat4_t translation_matrix = mat4_translation(
      mesh.translation.x, mesh.translation.y, mesh.translation.z);

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

      /*transformed_vertex = vec3_rotate_x(transformed_vertex,
       * mesh.rotation.x);*/
      /*transformed_vertex = vec3_rotate_y(transformed_vertex,
       * mesh.rotation.y);*/
      /*transformed_vertex = vec3_rotate_z(transformed_vertex,
       * mesh.rotation.z);*/
      /**/

      transformed_vertex = mat4_mul_vec(scale_matrix, transformed_vertex);
      transformed_vertex = mat4_mul_vec(translation_matrix, transformed_vertex);

      /*transformed_vertex.z += 5;*/

      transformed_vertices[j] = transformed_vertex;
    }

    // TODO: check backface culling
    if (cull_method == CULL_BACKFACE) {
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

      // bypass the triangles that are looking away from the camera
      if (dot_normal_camera < 0) {
        continue;
      }
    }

    vec2_t projected_points[3];

    // loop all 3 vertices of the face to perform projection
    for (int j = 0; j < 3; j++) {
      projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]));

      // sclae and translate the projected points to the middle of the screen
      projected_points[j].x += (window_width / 2.0);
      projected_points[j].y += (window_height / 2.0);
    }

    // Calc the avg depth for each face based on the vertices
    // after transformation
    float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z +
                       transformed_vertices[2].z) /
                      3.0;

    triangle_t projected_triangle = {.points = {{
                                                    projected_points[0].x,
                                                    projected_points[0].y,
                                                },
                                                {
                                                    projected_points[1].x,
                                                    projected_points[1].y,
                                                },
                                                {
                                                    projected_points[2].x,
                                                    projected_points[2].y,
                                                }},
                                     .color = mesh_face.color,
                                     .avg_depth = avg_depth};

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

    // save the projected triangle in the array of triangles to render
    /*triangles_to_render[i] = projected_triangle;*/
    array_push(triangles_to_render, projected_triangle);
  }
}

void render(void) {
  draw_grid(0xFF555555);

  // loop all projected triangles and render them
  int num_triangles = array_length(triangles_to_render);

  for (int i = 0; i < num_triangles; i++) {
    triangle_t triangle = triangles_to_render[i];

    if (render_method == RENDER_FILL_TRIANGLE ||
        render_method == RENDER_FILL_TRIANGLE_WIRE)
      draw_filled_triangle(triangle.points[0].x, triangle.points[0].y,
                           triangle.points[1].x, triangle.points[1].y,
                           triangle.points[2].x, triangle.points[2].y,
                           triangle.color);

    if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX ||
        render_method == RENDER_FILL_TRIANGLE_WIRE)
      draw_triangle(triangle.points[0].x, triangle.points[0].y,
                    triangle.points[1].x, triangle.points[1].y,
                    triangle.points[2].x, triangle.points[2].y, COLOR_BLUE);

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
  array_free(mesh.faces);
  array_free(mesh.vertices);
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
