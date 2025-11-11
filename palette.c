#include "palette.h"

// colour index_to_uvw(char index) {

// }

uint8_t uvwa_to_index(colour uvwa, palette pal) {
	// pack top bits from u,v,w into 8-bit index: [u_bits][v_bits][w_bits]
	uint8_t u = uvwa.u;
	uint8_t v = uvwa.v;
	uint8_t w = uvwa.w;
	uint8_t index = 0;
	if (pal.u_bits > 0) {
		u = u >> (8 - pal.u_bits);
		index |= (uint8_t)(u << (pal.v_bits + pal.w_bits));
	}
	if (pal.v_bits > 0) {
		v = v >> (8 - pal.v_bits);
		index |= (uint8_t)(v << pal.w_bits);
	}
	if (pal.w_bits > 0) {
		w = w >> (8 - pal.w_bits);
		index |= w;
	}
	return index;
}

// uint32_t index_to_rgba(char index, palette pal) {
// 	colour pal_colour = palette.colours[index];
// 	return (uint32_t)pal_colour;
// }

colour index_to_colour(uint8_t index, palette pal) {
	return pal.colours[index];
}
