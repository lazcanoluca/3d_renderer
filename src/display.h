#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

// Represent colors in ARGB8888 format.
typedef uint32_t argb_t;

/*typedef struct {*/
/*  int window_width;*/
/*  int window_height;*/
/*} viewport_t;*/

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *color_buffer_texture;
extern argb_t *color_buffer;
extern int window_width;
extern int window_height;

bool init_window(void);
void destroy_window(void);
void clear_color_buffer(argb_t color);
void draw_grid(argb_t color);
void draw_rect(int x, int y, int width, int height, argb_t color);
void draw_pixel(int x, int y, argb_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                   argb_t color);
void draw_line(int x0, int y0, int x1, int y1, argb_t color);
void render_color_buffer(void);

#endif
