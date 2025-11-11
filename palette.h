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

#endif
