#ifndef PALETTE_H
#define PALETTE_H

#include "types.h"

typedef struct colour {
    uint8_t u, v, w, a;
} colour;

typedef struct uvwa_colour {
    uint8_t u, v, w, a;
} uvwa_colour;

typedef struct rgb_colour {
    uint8_t r, g, b;
} rgb_colour;

typedef struct palette {
	uint8_t u_bits, v_bits, w_bits;
	rgb_colour colours[256];
} palette;

uint8_t uvwa_to_index(
	const uvwa_colour *uvwa,
	const palette *pal);

rgb_colour index_to_rgb(
	uint8_t index,
	palette pal);

#endif
