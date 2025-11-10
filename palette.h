#ifndef PALETTE_H
#define PALETTE_H

#include "render.h"

typedef struct palette {
	unsigned char u_bits, v_bits, w_bits;
	colour colours[256];
} palette;

// Core palette functions (platform-agnostic, no stdlib)
uint8_t uvwa_to_index(colour uvwa, palette pal);
colour index_to_colour(uint8_t index, palette pal);
void load_palette_from_rgb(uint8_t* rgb_data, palette* pal);
void convert_uvw_to_paletted(colour* uvw_buffer, uint8_t* paletted_buffer, palette* pal, int width, int height);
void convert_paletted_to_rgb(uint8_t* paletted_buffer, colour* rgb_buffer, palette* pal, int width, int height);

// Platform-specific loading functions (defined in separate files)
#ifdef SDL_PLATFORM
bool load_palette_from_png_sdl(const char* filename, palette* pal);
#endif

// Low-level PNG PLTE reader (platform-agnostic, no stdlib)
// Load PNG file into memory first, then call this function
int load_palette_from_png_data(uint8_t* file_data, int file_size, palette* pal);

#endif
