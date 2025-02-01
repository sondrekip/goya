#include "palette.h"

// colour index_to_uvw(char index) {

// }

uint8_t uvwa_to_index(colour uvwa, palette pal) {
	// generate a palette index from uvw values
	uint8_t u = uvwa.u;
	uint8_t v = uvwa.v;
	uint8_t w = uvwa.w;

	// take first pal.u_bits bits from uvwa.u
	// right shift to keep only upper bits
	u >> (8 - pal.u_bits);
	u << (8 - pal.u_bits);

	// add first pal.v_bits bits from uvwa.v
	v >> (8 - pal.v_bits);
	v << pal.w_bits;

	// add first pal.w_bits bits from uvwa.w
	w >> (8 - pal.w_bits);

	// or bits together
	uint8_t index = u | v | w;
	return index;
}

uint32_t index_to_rgba(char index, palette pal) {
	colour pal_colour = palette.colours[index];
	return (uint32_t)pal_colour;
}

