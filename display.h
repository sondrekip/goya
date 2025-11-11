#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "render.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_PixelFormat* g_fmt;

bool init_SDL();
void close_SDL();
void render_to_texture();

#endif
