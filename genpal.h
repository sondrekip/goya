#ifndef GENPAL_H
#define GENPAL_H

#include "palette.h"

/* Build a 256-entry palette from key_colours for given bit layout.
   Requires: u_bits + v_bits == 8. (w_bits is 0 for now.) */
void generate_uv_palette(palette *pal, int u_bits, int v_bits);

#endif

// #include "palette.h"
//
// static const rgb_colour key_colours[3][3]
// static uint8_t lerp_channel(uint8_t a, uint8_t b, float t);
// static rgb_colour lerp_color(rgb_colour c0, rgb_colour c1, float t);
// static void split_axis(float t, int *i0, int *i1, float *ft);
// static rgb_colour sample_colour(int u_idx, int v_idx, int u_size, int v_size);
// static void build_palette(rgb_colour palette[256], int u_bits, int v_bits);
