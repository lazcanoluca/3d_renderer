#include "display.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define N_POINTS 729

/*const int N_POINTS = 9 * 9 * 9;*/
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t cube_rotation = {0, 0, 0};

vec3_t camera_position = {0, 0, -5};

float fov_factor = 640;

#define COLOR_BLACK 0xFF000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE 0xFF0000FF
#define COLOR_YELLOW 0xFFFFFF00

bool is_running = false;

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

  int point_count = 0;

  for (float x = -1; x <= 1; x += 0.25) {
    for (float y = -1; y <= 1; y += 0.25) {
      for (float z = -1; z <= 1; z += 0.25) {
        vec3_t new_point = {.x = x, .y = y, .z = z};
        cube_points[point_count++] = new_point;
      }
    }
  }
}

vec2_t project(vec3_t point) {
  vec2_t projected = {
      .x = (fov_factor * point.x) / point.z,
      .y = (fov_factor * point.y) / point.z,
  };

  return projected;
}

void update(void) {
  cube_rotation.y += 0.01;

  for (int i = 0; i < N_POINTS; i++) {
    vec3_t point = cube_points[i];

    vec3_t transformed_point = vec3_rotate_y(point, cube_rotation.y);

    // move points away from camera
    transformed_point.z -= camera_position.z;

    projected_points[i] = project(transformed_point);
  }
}

void render(void) {
  draw_grid(COLOR_WHITE);

  // loop all projected points and render them
  for (int i = 0; i < N_POINTS; i++) {
    vec2_t point = projected_points[i];
    draw_rect(point.x + (window_width / 2.0), point.y + (window_height / 2.0),
              4, 4, COLOR_YELLOW);
  }

  // render to sdl buffer
  render_color_buffer();

  // draw for next frame
  clear_color_buffer(COLOR_BLACK);

  SDL_RenderPresent(renderer);
}

int main(void) {
  is_running = init_window();

  setup();

  /*vec3_t a_vector = {2.0, 3.0, -4.0};*/

  while (is_running) {
    process_input();
    update();
    render();
  }

  destroy_window();

  return 0;
}
