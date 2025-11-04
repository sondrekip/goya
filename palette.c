#include "palette.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>

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

bool load_palette_from_png(const char* filename, palette* pal) {
	SDL_Surface* surface = IMG_Load(filename);
	if (surface == NULL) {
		fprintf(stderr, "Failed to load palette image: %s\n", IMG_GetError());
		return false;
	}

	// Check if image is 256x1 or 16x16 (standard palette sizes)
	if (surface->w != 256 || surface->h != 1) {
		if (surface->w == 16 && surface->h == 16) {
			// Convert 16x16 palette to 256x1 by reading row by row
			SDL_LockSurface(surface);
			uint8_t* pixels = (uint8_t*)surface->pixels;
			int bpp = surface->format->BytesPerPixel;
			
			for (int i = 0; i < 256; i++) {
				int x = i % 16;
				int y = i / 16;
				uint8_t* pixel = pixels + (y * surface->pitch + x * bpp);
				
				uint32_t pixel_value = 0;
				if (bpp == 1) {
					pixel_value = *pixel;
				} else if (bpp == 2) {
					pixel_value = *(uint16_t*)pixel;
				} else if (bpp == 3) {
					pixel_value = pixel[0] | (pixel[1] << 8) | (pixel[2] << 16);
				} else if (bpp == 4) {
					pixel_value = *(uint32_t*)pixel;
				}
				
				uint8_t r, g, b, a;
				SDL_GetRGBA(pixel_value, surface->format, &r, &g, &b, &a);
				
				// Convert RGB to UVW format (treating RGB as UVW axes)
				pal->colours[i].u = r;
				pal->colours[i].v = g;
				pal->colours[i].w = b;
				pal->colours[i].a = a;
			}
			SDL_UnlockSurface(surface);
		} else {
			fprintf(stderr, "Palette image must be 256x1 or 16x16 pixels\n");
			SDL_FreeSurface(surface);
			return false;
		}
	} else {
		// Read 256x1 palette
		SDL_LockSurface(surface);
		uint8_t* pixels = (uint8_t*)surface->pixels;
		int bpp = surface->format->BytesPerPixel;
		
		for (int i = 0; i < 256; i++) {
			uint8_t* pixel = pixels + (i * bpp);
			
			uint32_t pixel_value = 0;
			if (bpp == 1) {
				pixel_value = *pixel;
			} else if (bpp == 2) {
				pixel_value = *(uint16_t*)pixel;
			} else if (bpp == 3) {
				pixel_value = pixel[0] | (pixel[1] << 8) | (pixel[2] << 16);
			} else if (bpp == 4) {
				pixel_value = *(uint32_t*)pixel;
			}
			
			uint8_t r, g, b, a;
			SDL_GetRGBA(pixel_value, surface->format, &r, &g, &b, &a);
			
			// Convert RGB to UVW format (treating RGB as UVW axes)
			pal->colours[i].u = r;
			pal->colours[i].v = g;
			pal->colours[i].w = b;
			pal->colours[i].a = a;
		}
		SDL_UnlockSurface(surface);
	}

	SDL_FreeSurface(surface);
	return true;
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

