#include "display.h"
#include "render.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_PixelFormat* g_fmt = NULL;

bool init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    window = SDL_CreateWindow("Vector Drawing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (texture == NULL) {
        fprintf(stderr, "Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    g_fmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

    if (g_fmt == NULL) {
      fprintf(stderr, "Pixel format alloc failed! SDL_Error: %s\n", SDL_GetError());
      SDL_DestroyTexture(texture);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return false;
    }
    return true;
}

void close_SDL() {
    if (g_fmt != NULL) SDL_FreeFormat(g_fmt);
    if (texture != NULL) SDL_DestroyTexture(texture);
    if (renderer != NULL) SDL_DestroyRenderer(renderer);
    if (window != NULL) SDL_DestroyWindow(window);
    SDL_Quit();
}

// change: add base_layer to arguments:
void render_to_texture() {
    SDL_UpdateTexture(texture, NULL, rgb_base_layer, WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  
    // Map each pixel via SDL to avoid any endianness/packing issues.
    void* pixels = NULL; int pitch = 0;
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
      fprintf(stderr, "SDL_LockTexture failed: %s\n", SDL_GetError());
      return;
    }
    Uint32* dst = (Uint32*)pixels;
    const int stride = pitch / 4; // RGBA8888
    for (int y = 0; y < HEIGHT; ++y) {
      Uint32* row = dst + y * stride;
      colour* src = &base_layer[y * WIDTH];
      for (int x = 0; x < WIDTH; ++x) {
        // Currently you treat u,v,w as r,g,b (keep that mapping here):
        row[x] = SDL_MapRGBA(g_fmt, src[x].u, src[x].v, src[x].w, src[x].a);
      }
    }
    SDL_UnlockTexture(texture);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}
