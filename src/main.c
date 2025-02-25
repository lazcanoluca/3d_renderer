#include "../lib/array.h"
#include "display.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"
#include <SDL2/SDL.h>
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

vec3_t camera_position = {0, 0, -5};

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
    break;
  }
}

void setup(void) {
  color_buffer =
      (argb_t *)malloc(sizeof(argb_t) * window_width * window_height);

  // TODO: check malloc

  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);

  /*load_cube_mesh_data();*/
  load_obj_file_data("./assets/f22.obj");
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

  // loop all triangle faces
  for (int i = 0; i < array_length(mesh.faces); i++) {
    face_t mesh_face = mesh.faces[i];

    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];

    triangle_t projected_triangle;

    // loop all 3 vertices of the face and apply transformations
    for (int j = 0; j < 3; j++) {
      vec3_t transformed_vertex = face_vertices[j];

      transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
      transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
      transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

      transformed_vertex.z -= camera_position.z;

      vec2_t projected_point = project(transformed_vertex);

      // sclae and translate the projected points to the middle of the screen
      projected_point.x += (window_width / 2.0);
      projected_point.y += (window_height / 2.0);

      projected_triangle.points[j] = projected_point;
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

    draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, COLOR_YELLOW);
    draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, COLOR_YELLOW);
    draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, COLOR_YELLOW);

    // draw unfilled tri
    draw_triangle(triangle.points[0].x, triangle.points[0].y,
                  triangle.points[1].x, triangle.points[1].y,
                  triangle.points[2].x, triangle.points[2].y, COLOR_YELLOW);
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
