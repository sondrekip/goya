#include <stdio.h>
#include "render.h"
#include "palette.h"

colour base_layer[WIDTH * HEIGHT];

colour red = (colour){255,100,0,255};
colour black = (colour){255,0,0,0};
colour white = (colour){255, 255, 255, 255};
colour blue = (colour){255,255,100,0};
colour green = (colour){255,100,255,0};

static uint32_t lcg_seed = 1;
uint32_t lcg_rand() {
// linear congruential generator
    lcg_seed = (lcg_seed * 1664525 + 1013904223) % 0xFFFFFFFF;
    return lcg_seed;
}

float my_sqrt(float x) {
// newton-raphson square root
    if (x <= 0) return 0;

    float guess = x / 2.0;
    float epsilon = 0.1; // precision
    while ((guess * guess - x) > epsilon || (x - guess * guess) > epsilon) {
        guess = (guess + x / guess) / 2.0;
    }
    return guess;
}

float my_log(float x) {
// bakhshali's approximation logarithm
    if (x <= 0) return -1; // error for non-positive numbers
    float y = (x - 1) / (x + 1);
    float y2 = y * y;
    float sum = 0.0;
    float term = y;
    int n = 1;

    while (term > 0.1 || term < -0.00001) {
        sum += term;
        term *= y2;
        term *= (2.0 * n - 1) / (2.0 * n + 1);
        n++;
    }

    return 2.0 * sum;
}

float gaussian() {
    float u, v, r;
    do {
        u = ((float)lcg_rand() / 0xFFFFFFFF) * 2.0 - 1.0;
        v = ((float)lcg_rand() / 0xFFFFFFFF) * 2.0 - 1.0;
        r = u * u + v * v;
    } while (r == 0 || r > 1); // avoid zero and out-of-bounds

    float c = my_sqrt(-2.0 * my_log(r) / r);
    return u * c;
}

float dist(vertex a, vertex b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float distance_squared = dx * dx + dy * dy;
    
    // safety check:
    if (distance_squared < 0) {
        return 0;
    }
    
    return my_sqrt(distance_squared);
}

vertex midpoint(vertex a, vertex b) {
    // returns a vertex between a and b
    vertex mid;
    mid.x = (a.x + b.x) / 2;
    mid.y = (a.y + b.y) / 2;
    mid.width = (a.width + b.width) / 2;
    mid.opacity = (a.opacity + b.opacity) / 2;
    mid.hardness = (a.hardness + b.hardness) / 2;
    return mid;
}

vertex midpoint_with_noise(vertex a, vertex b, float displacement_factor) {
    // returns a vertex between a and b with added displacement
    vertex mid;
    mid.x = average_with_noise(a.x, b.x, displacement_factor);
    mid.y = average_with_noise(a.y, b.y, displacement_factor);
    mid.width = average_with_noise(a.width, b.width, displacement_factor);
    mid.opacity = average_with_noise(a.opacity, b.opacity, displacement_factor);
    mid.hardness = average_with_noise(a.hardness, b.hardness, displacement_factor);
    return mid;
}

int average_with_noise(int a, int b, float displacement_factor) {
    int average = (a + b) / 2;
    int noise_range = average * displacement_factor;
    // noise range should not be <= 0:
    if (noise_range <= 0) noise_range = 1;
    int average_with_noise = average + lcg_rand() % noise_range - noise_range / 2;
    return average_with_noise;
}

void perpendicular_vector(vertex a, vertex b, float *dx, float *dy) {
    *dx = b.y - a.y;
    *dy = a.x - b.x;
    float length = my_sqrt((*dx) * (*dx) + (*dy) * (*dy));
    // avoid division by zero:
    if (length == 0) {
        *dx = 0;
        *dy = 0;
    } else {
        *dx /= length;
        *dy /= length;
    }
}

void apply_perpendicular_displacement(vertex* mid, vertex a, vertex b, float displacement) {
    float dx, dy;
    perpendicular_vector(a, b, &dx, &dy);
    mid->x += (int32_t)(dx * displacement);
    mid->y += (int32_t)(dy * displacement);
}

// int noisy_neighbour_distance(vertex a, vertex b, vertex c) {
//     int dist1 = dist(a, b);
//     int dist2 = dist(b, c);
//     int average = average_with_noise(dist1, dist2, 0.1);
//     return average;
// }

// vertex displacement_from_dist(int dist, float displacement_factor) {
//     vertex displacement;
//     displacement.x = gaussian() * dist * 0.1;
//     displacement.y = gaussian() * dist * 0.1;
//     return displacement;
// }

void apply_displacement(vertex* a, vertex displacement) {
    // printf("calling apply_displacement\n");
    int disp_x = a->x + displacement.x;
    int disp_y = a->y + displacement.y;
    // a->x = 
    // a->y = a->y + displacement.y;
    // printf("x: %d\n", disp_x);
    // printf("y: %d\n", disp_y);
    a->x = disp_x;
    a->y = disp_y;
    // printf("finished apply_displacement\n");
    return;
}

void displace_vertex(vertex *a, float radius, float displacement_factor) {
    // float radius = dist(*a, *b);
    float softness = (float)(255 - a->hardness) / 255.0;

    vertex displacement;
    displacement.x = gaussian() * radius * displacement_factor * softness;
    displacement.y = gaussian() * radius * displacement_factor * softness;
    a->x = a->x + displacement.x;
    a->y = a->y + displacement.y;
}

// float hardness_to_softness(uint8_t hardness, float softness) {
//     float softness = (float)(255 - hardness) / 255.0;
//     return softness;
// }

void subdivide_poly(vertex *poly,
                    vertex *sub_poly,
                    int n_vertices,
                    int n_levels,
                    float displacement_factor) {
    // if this is the last level, copy the final polygon and return:
    if (n_levels == 0) {
        for (int i = 0; i < n_vertices; i++) {
            sub_poly[i] = poly[i];
        }
        return;
    }
    // otherwise create a new polygon and fill in intermediate vertices:
    else {
        int new_n_vertices = n_vertices * 2;
        vertex new_poly[new_n_vertices];

        for (int i = 0; i < n_vertices; i++) {
            vertex a = poly[i];
            // if a is the last vertex, b is the first vertex:
            vertex b = (i == n_vertices - 1) ? poly[0] : poly[i + 1];

            // displace vertex a:
            float edge_length = dist(a, b);
            // vertex disp_vertex = displacement_from_dist(edge_length, displacement_factor);
            // apply_displacement(&a, disp_vertex);
            displace_vertex(&a, edge_length, displacement_factor);

            new_poly[i * 2] = a;

            float softness_factor = (1 - (a.hardness + b.hardness) / 2.0 / 255.0) * displacement_factor;
            vertex mid = midpoint_with_noise(a, b, softness_factor);
            // vertex mid = midpoint(a, b);
            float displacement = displacement_factor * gaussian() * edge_length * softness_factor;
            apply_perpendicular_displacement(&mid, a, b, displacement);

            new_poly[i * 2 + 1] = mid;
        }

        // recursively subdivide the new polygon:
        subdivide_poly(new_poly, sub_poly, new_n_vertices, n_levels - 1, displacement_factor);
    }
}

void clear(colour *layer, colour colour) {
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        layer[i] = colour;
    }
}

void draw_pixel(colour *layer, int x, int y, colour colour) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        layer[y * WIDTH + x] = colour;
    }
}

void draw_line(colour *layer, int x0, int y0, int x1, int y1, colour colour) {
    int dx = my_abs(x1 - x0);
    int dy = my_abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (true) {
        draw_pixel(layer, x0, y0, colour);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void draw_poly(colour *layer, vertex *poly_array, int length, colour colour) {
    int minY = HEIGHT, maxY = 0;
    // Find the vertical range of the polygon
    for (int i = 0; i < length; i++) {
        if (poly_array[i].y < minY) minY = poly_array[i].y;
        if (poly_array[i].y > maxY) maxY = poly_array[i].y;
    }

    // Iterate over each scanline
    for (int y = minY; y <= maxY; y++) {
        int nodes = 0;
        int nodeX[length];
        // Find intersections of the polygon with the scanline
        for (int i = 0; i < length; i++) {
            vertex v1 = poly_array[i];
            vertex v2 = poly_array[(i + 1) % length];
            if ((v1.y < y && v2.y >= y) || (v2.y < y && v1.y >= y)) {
                nodeX[nodes++] = v1.x + (y - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
            }
        }

        // Sort the nodes in ascending order
        for (int i = 0; i < nodes - 1; i++) {
            for (int j = i + 1; j < nodes; j++) {
                if (nodeX[i] > nodeX[j]) {
                    int temp = nodeX[i];
                    nodeX[i] = nodeX[j];
                    nodeX[j] = temp;
                }
            }
        }

        // Draw the scanline between node pairs
        for (int i = 0; i < nodes; i += 2) {
            if (nodeX[i] >= WIDTH) break;
            if (nodeX[i + 1] > 0) {
                if (nodeX[i] < 0) nodeX[i] = 0;
                if (nodeX[i + 1] > WIDTH) nodeX[i + 1] = WIDTH;
                for (int x = nodeX[i]; x < nodeX[i + 1]; x++) {
                    draw_pixel(layer, x, y, colour);
                }
            }
        }
    }
}

colour blend_colours(colour base_colour, colour draw_colour) {
    float a1 = (float)base_colour.a / 255.0;
    float a2 = (float)draw_colour.a / 255.0;
    float blend_a = a1 + a2 - a1 * a2;

    colour blend_colour;
    blend_colour.u = (draw_colour.u * a2 + base_colour.u * a1 * (1 - a2)) / blend_a;
    blend_colour.v = (draw_colour.v * a2 + base_colour.v * a1 * (1 - a2)) / blend_a;
    blend_colour.w = (draw_colour.w * a2 + base_colour.w * a1 * (1 - a2)) / blend_a;
    blend_colour.a = (uint8_t)(blend_a * 255.0);

    return blend_colour;
}

void merge_buffers(colour *base_buffer, colour *draw_buffer, int width, int height) {
    for (int i = 0; i < width*height; i++) {
        uint8_t draw_alpha = draw_buffer[i].a;
        if (draw_alpha == 0) {
            continue;
        }
        else if (draw_alpha == 255) {
            base_buffer[i] = draw_buffer[i];
        }
        else {
            base_buffer[i] = blend_colours(base_buffer[i], draw_buffer[i]);
        }
    }
}

void subdivide_paint(colour *buffer, 
                    vertex *poly, 
                    int n_vertices, 
                    int n_subdivisions, 
                    int n_translucency_levels,
                    colour fill_colour,
                    int width,
                    int height) {

    // allocate array for 2^n vertices:
    int final_n_vertices = n_vertices * (1 << n_subdivisions);
    vertex sub_poly[final_n_vertices];
    int u = fill_colour.u;
    int v = fill_colour.v;
    int w = fill_colour.w;
    int a = fill_colour.a;
    // printf("%d %d %d %d\n", u, v, w, a);

    // int polysize = (int)sizeof(poly);
    // int vertsize = (int)sizeof(vertex);
    // printf("poly size: %d\n", polysize);
    // printf("vertex size: %d\n", vertsize);

    int max_opacity = fill_colour.a;
    int init_opacity = max_opacity * 2 / n_translucency_levels;
    // int opacity_step = max_opacity * 2 / n_translucency_levels;
    int opacity_step = max_opacity / n_translucency_levels;
    float exp_opacity_factor = 1 + (float) opacity_step / max_opacity;
    // printf("max opacity: %d step: %d\n", max_opacity, opacity_step);

    colour draw_layer[width * height];
    clear(draw_layer, (colour){0,0,0,0});
    // fill_colour.a = init_opacity;
    fill_colour.a = opacity_step;

    // for each subdivision, add to draw layer:
    for (int i = 0; i < n_translucency_levels; i++) {
        // adjust opacity step to get closer to opaque colour:
        // opacity_step *= 1.1;
        // opacity_step += opacity_step;
        // fill_colour.a += opacity_step;
        fill_colour.a *= exp_opacity_factor;
        // printf("opacity: %d\n", fill_colour.a);
        subdivide_poly(poly, sub_poly, n_vertices, n_subdivisions, 0.4);
        draw_poly(draw_layer, sub_poly, final_n_vertices, fill_colour);
        merge_buffers(buffer, draw_layer, width, height);
        clear(draw_layer, (colour){0,0,0,0});
    }
}

void mask(colour *buffer, colour *mask, int width, int height) {
    for (int i = 0; i < width*height; i++) {
        // int mask_alpha = mask[i].a;
        float mask_alpha_factor = mask[i].a / 255.0;
        buffer[i].a = buffer[i].a * mask_alpha_factor;
    }
}

// void merge_buffers(colour *base_buffer, colour *draw_buffer, int width, int height) {
//     for (int i = 0; i < width*height; i++) {
//         uint8_t draw_alpha = draw_buffer[i].a;
//         if (draw_alpha == 0) {
//             continue;
//         }
//         else if (draw_alpha == 255) {
//             base_buffer[i] = draw_buffer[i];
//         }
//         else {
//             base_buffer[i] = blend_colours(base_buffer[i], draw_buffer[i]);
//         }
//     }
// }



// void draw_poly(colour *layer, vertex *poly_array, int length, colour colour) {

// Example draw_filled_polygon function (simple scanline fill algorithm)
// void draw_outline(vertex *poly, int n_vertices, colour fill_colour, colour *buffer, int width, int height) {
//     // Simple scanline fill algorithm implementation
//     // This is a placeholder function, and you may need to use a more sophisticated polygon filling algorithm
//     for (int i = 0; i < n_vertices; i++) {
//         int x0 = poly[i].x;
//         int y0 = poly[i].y;
//         int x1 = poly[(i + 1) % n_vertices].x;
//         int y1 = poly[(i + 1) % n_vertices].y;
        
//         // Draw line between points (x0, y0) and (x1, y1)
//         // This needs to be replaced with a proper polygon fill logic
//         draw_line(buffer, x0, y0, x1, y1, fill_colour);
//     }
// }

void draw_outline(colour *layer, vertex *poly_array, int length, colour colour) {
    for (int i = 0; i < length; i++) {
        int next = (i + 1) % length;
        draw_line(layer, poly_array[i].x, poly_array[i].y, poly_array[next].x, poly_array[next].y, colour);
    }
}
