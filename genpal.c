#include "palette.h"
#include <stdio.h>

// chroma-luma palette:
// static const rgb_colour key_colours[3][3] = {
//     { {  0,  0,  0}, {  0,  0,  0}, {  0,  0,  0} },
//     { { 85,143,163}, {135,102,135}, {185, 89, 68} },
//     { {255,255,255}, {255,255,255}, {255,255,255} }
// };
// additive palette:
static const rgb_colour key_colours[3][3] = {
    { {  0,  0,  0}, { 40, 14,128}, { 79, 28,255} },
    { { 128,14,43}, {167,28,170}, {167, 141, 255} },
    { {255,46,41}, {255,136,47}, {255,255,255} }
};

static uint8_t lerp_channel(uint8_t a, uint8_t b, float t)
{
    float v = a + (b - a) * t;
    if (v < 0.0f)   v = 0.0f;
    if (v > 255.0f) v = 255.0f;
    return (uint8_t)(v + 0.5f); /* round to nearest */
}

static rgb_colour lerp_color(rgb_colour c0, rgb_colour c1, float t)
{
    rgb_colour c;
    c.r = lerp_channel(c0.r, c1.r, t);
    c.g = lerp_channel(c0.g, c1.g, t);
    c.b = lerp_channel(c0.b, c1.b, t);
    return c;
}

/* t in [0,1], control points at 0, 0.5, 1 */
static void split_axis(float t, int *i0, int *i1, float *ft)
{
    if (t <= 0.5f) {
        *i0 = 0;
        *i1 = 1;
        *ft = t / 0.5f;          // 0..1 between 0 and 0.5
    } else {
        *i0 = 1;
        *i1 = 2;
        *ft = (t - 0.5f) / 0.5f; // 0..1 between 0.5 and 1
    }
}

static rgb_colour sample_colour(int u_idx, int v_idx, int u_size, int v_size)
{
    float u = (u_size > 1) ? (float)u_idx / (float)(u_size - 1) : 0.0f;
    float v = (v_size > 1) ? (float)v_idx / (float)(v_size - 1) : 0.0f;

    int ux0, ux1, vy0, vy1;
    float fu, fv;

    split_axis(u, &ux0, &ux1, &fu);
    split_axis(v, &vy0, &vy1, &fv);

    rgb_colour c00 = key_colours[vy0][ux0];
    rgb_colour c10 = key_colours[vy0][ux1];
    rgb_colour c01 = key_colours[vy1][ux0];
    rgb_colour c11 = key_colours[vy1][ux1];

    rgb_colour top    = lerp_color(c00, c10, fu);
    rgb_colour bottom = lerp_color(c01, c11, fu);
    return lerp_color(top, bottom, fv);
}

static void build_palette_rgb(rgb_colour palette[256], int u_bits, int v_bits)
{
    int u_size = 1 << u_bits;
    int v_size = 1 << v_bits;
    printf("build_palette u_size: %d\n", u_size);
    printf("build_palette v_size: %d\n", v_size);

    for (int idx = 0; idx < 256; ++idx) {
        int v_idx = idx & ((1 << v_bits) - 1);   // low bits
        int u_idx = idx >> v_bits;               // high bits
        palette[idx] = sample_colour(u_idx, v_idx, u_size, v_size);
    }
}

void generate_uv_palette(palette *pal, int u_bits, int v_bits)
{
    // for now I only use u+v
    pal->u_bits = (uint8_t)u_bits;
    pal->v_bits = (uint8_t)v_bits;
    pal->w_bits = 0;

    /* assert(u_bits + v_bits == 8); */

    build_palette_rgb(pal->colours, u_bits, v_bits);
}
