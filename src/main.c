#include "display.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define COLOR_BLACK 0xFF000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE 0xFF0000FF

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
}

void update(void) {}

void render(void) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  // render to sdl buffer
  render_color_buffer();

  // draw for next frame
  clear_color_buffer(COLOR_BLACK);
  draw_grid(COLOR_WHITE);
  draw_rect(333, 157, 79, 237, COLOR_BLUE);

  SDL_RenderPresent(renderer);
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

  return 0;
}
