#ifndef PALETTE_H
#define PALETTE_H

#include "render.h"

typedef struct palette {
	unsigned char u_bits, v_bits, w_bits;
	colour colours[256];
} palette;

uint8_t uvwa_to_index(
    colour uvwa, 
    palette pal);
colour index_to_colour(
    uint8_t index, 
    palette pal);
bool load_palette_from_png(const char* filename, palette* pal);
void convert_uvw_to_paletted(colour* uvw_buffer, uint8_t* paletted_buffer, palette* pal, int width, int height);
void convert_paletted_to_rgb(uint8_t* paletted_buffer, colour* rgb_buffer, palette* pal, int width, int height);

#endif
