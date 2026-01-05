#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

// Represent colors in ARGB8888 format.
typedef uint32_t argb_t;

enum cull_method { CULL_NONE, CULL_BACKFACE };

extern enum cull_method cull_method;
void cull_method_toggle(enum cull_method *cull_method);

enum render_method {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE,
};

extern enum render_method render_method;
void render_method_toggle(enum render_method *render_method);

/*typedef struct {*/
/*  int window_width;*/
/*  int window_height;*/
/*} viewport_t;*/

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *color_buffer_texture;
extern argb_t *color_buffer;
extern float *z_buffer;
extern int window_width;
extern int window_height;

bool init_window(void);
void destroy_window(void);
void clear_color_buffer(argb_t color);
void clear_z_buffer();
void draw_grid(argb_t color);
void draw_rect(int x, int y, int width, int height, argb_t color);
void draw_pixel(int x, int y, argb_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                   argb_t color);
void draw_line(int x0, int y0, int x1, int y1, argb_t color);
void render_color_buffer(void);

#endif
