#include "palette.h"
// Platform-agnostic core palette functions
// No standard library dependencies for Amiga compatibility

uint8_t uvwa_to_index(colour uvwa, palette pal) {
	// generate a palette index from uvw values
	// Pack bits: [u_bits][v_bits][w_bits] = 8 bits total
	// Example: u_bits=5, v_bits=3, w_bits=0
	// Layout: [uuuuu][vvv][ww] = [7-3][2-0][none]
	
	uint8_t u = uvwa.u;
	uint8_t v = uvwa.v;
	uint8_t w = uvwa.w;
	
	uint8_t index = 0;
	
	// Extract upper u_bits from u and place in highest position
	// Shift right to get only the upper u_bits, then shift left to position
	if (pal.u_bits > 0) {
		u = u >> (8 - pal.u_bits);  // Get upper u_bits
		index |= (u << (pal.v_bits + pal.w_bits));  // Place in high bits
	}
	
	// Extract upper v_bits from v and place in middle
	if (pal.v_bits > 0) {
		v = v >> (8 - pal.v_bits);  // Get upper v_bits
		index |= (v << pal.w_bits);  // Place after w_bits
	}
	
	// Extract upper w_bits from w and place in lowest position
	if (pal.w_bits > 0) {
		w = w >> (8 - pal.w_bits);  // Get upper w_bits
		index |= w;  // Place in lowest bits
	}
	
	return index;
}

colour index_to_colour(uint8_t index, palette pal) {
	return pal.colours[index];
}

// Load palette from raw RGB data (platform-agnostic)
// rgb_data should be 256 * 3 bytes (R, G, B for each entry)
void load_palette_from_rgb(uint8_t* rgb_data, palette* pal) {
	for (int i = 0; i < 256; i++) {
		pal->colours[i].u = rgb_data[i * 3 + 0];  // R -> U
		pal->colours[i].v = rgb_data[i * 3 + 1];  // G -> V
		pal->colours[i].w = rgb_data[i * 3 + 2];  // B -> W
		pal->colours[i].a = 255;  // Full opacity
	}
}

void convert_uvw_to_paletted(colour* uvw_buffer, uint8_t* paletted_buffer, palette* pal, int width, int height) {
	for (int i = 0; i < width * height; i++) {
		paletted_buffer[i] = uvwa_to_index(uvw_buffer[i], *pal);
	}
}

void convert_paletted_to_rgb(uint8_t* paletted_buffer, colour* rgb_buffer, palette* pal, int width, int height) {
	for (int i = 0; i < width * height; i++) {
		rgb_buffer[i] = index_to_colour(paletted_buffer[i], *pal);
		// Preserve alpha from original if needed, or use palette alpha
		// rgb_buffer[i].a = paletted_buffer[i] < 256 ? pal->colours[paletted_buffer[i]].a : 0;
	}
}

