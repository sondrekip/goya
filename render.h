#ifndef RENDER_H
#define RENDER_H

#include "types.h"
#include "palette.h"

#define WIDTH 640
#define HEIGHT 512

// typedef signed char int8_t;
// typedef unsigned char uint8_t;
// typedef signed short int16_t;
// typedef unsigned short uint16_t;
// typedef signed int int32_t;
// typedef unsigned int uint32_t;
// typedef unsigned char my_bool;
// #define true 1
// #define false 0
// #define my_abs(x) ((x) < 0 ? -(x) : (x))

// typedef struct colour {
//     // uint8_t a, w, v, u;
//     // uint8_t u, v, w, a;
//     uint8_t a, u, v, w;
// } colour;

typedef struct vertex {
    int x, y;
    uint8_t hardness, width, opacity;
} vertex;

typedef struct fill_vertex {
    int x, y;
    uint8_t hardness;
} fill_vertex;

/*typedef struct line_vertex {*/
/*    int x, y;*/
/*    uint8_t hardness, width, opacity;*/
/*} line_vertex;*/

// extern colour base_layer[WIDTH * HEIGHT];
extern uvwa_colour base_layer[];

uint32_t lcg_rand();
float my_sqrt(float x);
float my_log(float x);
float gaussian();

void clear(
    uvwa_colour *layer, 
    uvwa_colour colour);
void draw_pixel(
    uvwa_colour *layer, 
    int x, 
    int y, 
    uvwa_colour colour);
void draw_line(
    uvwa_colour *layer, 
    int x0, 
    int y0, 
    int x1, 
    int y1, 
    uvwa_colour colour);
void draw_poly(
    uvwa_colour *layer, 
    vertex *poly_array, 
    int length, 
   uvwa_colour colour);
uvwa_colour blend_colours(
   uvwa_colour base_colour, 
   uvwa_colour draw_colour);
void merge_buffers(
   uvwa_colour *base_buffer, 
   uvwa_colour *draw_buffer, 
    int width, 
    int height);
// int blend_alpha(float base_alpha, float draw_alpha);
// void merge_to_base(colour *base_buffer, colour *draw_buffer, int width, int height);
void draw_filled_polygon(
    vertex *poly, 
    int n_vertices, 
   uvwa_colour fill_color, 
   uvwa_colour *buffer, 
    int width, 
    int height);
void draw_outline(
   uvwa_colour *layer, 
    vertex *poly_array, 
    int length, 
   uvwa_colour colour);
float dist(
    vertex a, 
    vertex b);
int average_with_noise(
    int a, 
    int b, 
    float displacement_factor);
vertex midpoint_with_noise(
    vertex a, 
    vertex b, 
    float displacement_factor);
void perpendicular_vector(
    vertex a, 
    vertex b, 
    float* dx, 
    float* dy);
void apply_perpendicular_displacement(
    vertex* mid, 
    vertex a, 
    vertex b, 
    float displacement);
vertex* subdivide_shape(
    vertex* shape, 
    int num_vertices, 
    int noise_divisor);
vertex displacement_from_dist(
    int dist, 
    float displacement_factor);
void apply_displacement(
    vertex* a, 
    vertex displacement);
void displace_vertex(
    vertex *a, 
    float radius, 
    float displacement_factor);
void subdivide_poly(
    vertex *poly, 
    vertex *sub_poly, 
    int n_vertices, 
    int n_levels, 
    float displacement_factor);
void subdivide_paint(
   uvwa_colour *buffer, 
    vertex *poly, 
    int n_vertices, 
    int n_subdivisions, 
    int n_translucency_levels,
   uvwa_colour colour,
    int width,
    int height);
void mask(
   uvwa_colour *buffer, 
   uvwa_colour *mask, 
    int width, 
    int height);
void uvwa_to_indexed_image(
    const uvwa_colour *uvwa_buffer,
    uint8_t *indexed_buffer,
    int width,
    int height,
    const palette *pal);
#endif // RENDER_H
