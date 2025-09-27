#ifndef RENDER_H
#define RENDER_H
#define WIDTH 640
#define HEIGHT 512

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef unsigned char my_bool;
#define true 1
#define false 0
#define my_abs(x) ((x) < 0 ? -(x) : (x))

typedef struct colour {
    uint8_t a, w, v, u;
} colour;

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
extern colour base_layer[];

uint32_t lcg_rand();
float my_sqrt(float x);
float my_log(float x);
float gaussian();

void clear(
    colour *layer, 
    colour colour);
void draw_pixel(
    colour *layer, 
    int x, 
    int y, 
    colour colour);
void draw_line(
    colour *layer, 
    int x0, 
    int y0, 
    int x1, 
    int y1, 
    colour colour);
void draw_poly(
    colour *layer, 
    vertex *poly_array, 
    int length, 
    colour colour);
colour blend_colours(
    colour base_colour, 
    colour draw_colour);
void merge_buffers(
    colour *base_buffer, 
    colour *draw_buffer, 
    int width, 
    int height);
// int blend_alpha(float base_alpha, float draw_alpha);
// void merge_to_base(colour *base_buffer, colour *draw_buffer, int width, int height);
void draw_filled_polygon(
    vertex *poly, 
    int n_vertices, 
    colour fill_color, 
    colour *buffer, 
    int width, 
    int height);
void draw_outline(
    colour *layer, 
    vertex *poly_array, 
    int length, 
    colour colour);
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
    colour *buffer, 
    vertex *poly, 
    int n_vertices, 
    int n_subdivisions, 
    int n_translucency_levels,
    colour colour,
    int width,
    int height);
void mask(
    colour *buffer, 
    colour *mask, 
    int width, 
    int height);
#endif // RENDER_H
