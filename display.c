#include "display.h"
#include "render.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "palette.h"
// #include "rgb-pal.h"

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
void render_to_texture(const rgb_colour *rgb_buffer) {
    void* pixels = NULL;
    int pitch = 0;
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
        fprintf(stderr, "SDL_LockTexture failed: %s\n", SDL_GetError());
        return;
    }

    Uint32* dst = (Uint32*)pixels;
    int stride = pitch / 4; // RGBA8888 texture

    for (int y = 0; y < HEIGHT; ++y) {
        Uint32* row = dst + y * stride;
        const rgb_colour* src = &rgb_buffer[y * WIDTH];
        for (int x = 0; x < WIDTH; ++x) {
            rgb_colour c = src[x];
            row[x] = SDL_MapRGB(g_fmt, c.r, c.g, c.b);
        }
    }

    SDL_UnlockTexture(texture);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

// todo:
void indexed_to_rgb_image(
    const uint8_t *indexed_buffer,
    rgb_colour *rgb_buffer,
    int width,
    int height,
    const palette *pal
) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = x + y * width;
            uint8_t idx = indexed_buffer[pos];
            rgb_buffer[pos] = pal->colours[idx];
        }
    }
}
