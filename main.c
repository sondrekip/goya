#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "render.h"
#include "display.h"
#include "palette.h"
#include "genpal.h"

int main(int argc, char* args[]) {
    init_SDL();

    if (window == NULL || renderer == NULL) {
        return 1;
    }

    palette pal;
    generate_uv_palette(&pal, 4, 4);   // example: 5 bits u, 3 bits v, w_bits = 0
    // uvwa_colour red = (uvwa_colour){ .u=255, .v=128, .w=20, .a=255 };
    uvwa_colour red = (uvwa_colour){ .u=0, .v=224, .w=0, .a=255 };
    uvwa_colour orange = (uvwa_colour){ .u=128, .v=255, .w=0, .a=255 };
    uvwa_colour black = (uvwa_colour){ .u=0, .v=0, .w=0, .a=255 };
    uvwa_colour white = (uvwa_colour){ .u=255, .v=255, .w=255, .a=255 };
    // uvwa_colour blue = (uvwa_colour){ .u=0, .v=255, .w=255, .a=128 };
    // uvwa_colour green = (uvwa_colour){ .u=0, .v=255, .w=100, .a=255 };
    uvwa_colour blue1 = (uvwa_colour){ .u=255, .v=0, .w=0, .a=255 };
    uvwa_colour blue2 = (uvwa_colour){ .u=255, .v=128, .w=0, .a=255 };
    uvwa_colour transparent = (uvwa_colour){ .u=0, .v=0, .w=0, .a=0 };

  
    // printf("red: %d %d %d %d\n", red.u, red.v, red.w, red.a);
    // printf("blue: %d %d %d %d\n", blue.u, blue.v, blue.w, blue.a);

    clear(base_layer, black);
    // uvwa_colour px0 = base_layer[0];
    // printf("pixel 0: %d %d %d %d\n", px0.u, px0.v, px0.w, px0.a);
    // clear(base_layer, black);
    // base_layer[50*WIDTH+50] = black;
    // base_layer[50*WIDTH+51] = black;
    // base_layer[50*WIDTH+52] = black;
    // draw_line(base_layer, 0, 0, 511, 511, test_colour); // White line

    int midx = WIDTH / 2;
    int midy = HEIGHT / 2;

    vertex diamond[4] = {
        {midx, 0, 255, 1, 255},
        {midx+midy, midy, 250, 1, 255},
        {midx, HEIGHT, 240, 1, 255},
        {midx-midy, midy, 150, 1, 255}
    };

    vertex diamond2[4] = {
        {midx, midy/2, 255, 1, 255},
        {midx+midy/2, midy, 250, 1, 255},
        {midx, HEIGHT-midy/2, 240, 1, 255},
        {midx-midy/2, midy, 150, 1, 255}
    };

    vertex poly1[4] = {
        {0, 0, 10, 1, 255}, // x, y, hardness, width, opacity
        {420, 106, 10, 1, 255},
        {420, 356, 80, 1, 255},
        {220, 356, 0, 1, 255}
    };

    vertex poly2[4] = {
        {120, 300, 25, 1, 255}, // x, y, hardness, width, opacity
        {510, 156, 20, 1, 255},
        {610, 456, 0, 1, 255},
        {220, 500, 0, 1, 255}
        // {10, 300, 150, 1, 255}
    };

    int polysize = sizeof(poly1)/sizeof(vertex);

    uvwa_colour draw_buffer1[WIDTH*HEIGHT];
    clear(draw_buffer1, (uvwa_colour){ .u=0, .v=0, .w=0, .a=255 });
    subdivide_paint(draw_buffer1, poly1, polysize, 3, 12, blue1, WIDTH, HEIGHT);
    // merge_buffers(base_layer, draw_buffer1, WIDTH, HEIGHT);

    uvwa_colour draw_buffer2[WIDTH*HEIGHT];
    clear(draw_buffer2, transparent);

    // int polysize = sizeof(poly)/sizeof(vertex);
    subdivide_paint(draw_buffer2, poly2, polysize, 3, 12, blue2, WIDTH, HEIGHT);
    // merge_buffers(base_layer, draw_buffer2, WIDTH, HEIGHT);

    merge_buffers(draw_buffer1, draw_buffer2, WIDTH, HEIGHT);
    // merge_buffers(base_layer, draw_buffer1, WIDTH, HEIGHT);

    uvwa_colour diamond_buffer[WIDTH*HEIGHT];
    clear(diamond_buffer, transparent);
    subdivide_paint(diamond_buffer, diamond, polysize, 3, 12, blue2, WIDTH, HEIGHT);
    mask(draw_buffer1, diamond_buffer, WIDTH, HEIGHT);
    merge_buffers(base_layer, draw_buffer1, WIDTH, HEIGHT);

    clear(draw_buffer1, transparent);
    subdivide_paint(draw_buffer1, poly1, polysize, 3, 14, orange, WIDTH, HEIGHT);
    clear(draw_buffer2, transparent);
    subdivide_paint(draw_buffer2, poly2, polysize, 3, 14, red, WIDTH, HEIGHT);
    merge_buffers(draw_buffer1, draw_buffer2, WIDTH, HEIGHT);

    clear(diamond_buffer, transparent);
    subdivide_paint(diamond_buffer, diamond2, polysize, 3, 12, red, WIDTH, HEIGHT);
    mask(draw_buffer1, diamond_buffer, WIDTH, HEIGHT);
    merge_buffers(base_layer, draw_buffer1, WIDTH, HEIGHT);

    // ---- render uvwa to indexed ----
    uint8_t indexed_buffer[WIDTH*HEIGHT];
    uvwa_to_indexed_image(base_layer, indexed_buffer, WIDTH, HEIGHT, &pal);

    // ---- render indexed to rgb ----
    rgb_colour rgb_buffer[WIDTH*HEIGHT];
    indexed_to_rgb_image(indexed_buffer, rgb_buffer, WIDTH, HEIGHT, &pal);

    render_to_texture(rgb_buffer);
    // base_layer[WIDTH*2+2] = (uvwa_colour){0,0,0,0};
    // clear(base_layer, (uvwa_colour){0,255,0,0});
    // uvwa_colour px0 = base_layer[0];
    // printf("pixel 0: %d %d %d %d\n", px0.u, px0.v, px0.w, px0.a);

    // event loop to keep the window open:
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        render_to_texture(rgb_buffer); // Keep rendering
    }

    close_SDL();
    return 0;
}


// #include "render.h"
// #include "display.h"
// #include <SDL2/SDL.h>
// #include <stdbool.h>
// #include <stdio.h>

// int main(int argc, char* args[]) {
//     // Initialize SDL
//     if (!initSDL()) {
//         fprintf(stderr, "Failed to initialize SDL\n");
//         return 1;
//     }

//     // Check for valid window and renderer
//     if (window == NULL || renderer == NULL) {
//         fprintf(stderr, "Failed to create window or renderer\n");
//         closeSDL();
//         return 1;
//     }

    // // Create a simple 4-vertex shape
    // vertex shape[4] = {
    //     {100, 100, 128, 1, 255}, // Vertex 1: x, y, hardness, width, opacity
    //     {200, 100, 200, 1, 255}, // Vertex 2
    //     {200, 200, 50, 1, 255},  // Vertex 3
    //     {100, 200, 150, 1, 255}  // Vertex 4
    // };

//     // Initialize the buffer
//     // uvwa_colour base_layer[WIDTH * HEIGHT];
//     clear(base_layer, (uvwa_colour){0, 0, 0, 0}); // Clear to transparent

//     // Add subdivided shapes to the base_layer
//     int n_vertices = 4;
//     int n_subs = 3;
//     int n_trans = 3;
//     int max_opacity = 200;

//     subdivide_paint(shape, n_vertices, n_subs, n_trans, max_opacity, base_layer, WIDTH, HEIGHT);

//     // Render the base_layer to the screen
//     SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
//     if (texture == NULL) {
//         fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());
//         closeSDL();
//         return 1;
//     }

//     bool quit = false;
//     SDL_Event e;

//     while (!quit) {
//         while (SDL_PollEvent(&e) != 0) {
//             if (e.type == SDL_QUIT) {
//                 quit = true;
//             }
//         }

//         // Update texture with base_layer
//         SDL_UpdateTexture(texture, NULL, base_layer, WIDTH * sizeof(uvwa_colour));

//         // Clear the renderer
//         SDL_RenderClear(renderer);

//         // Copy the texture to the renderer
//         SDL_RenderCopy(renderer, texture, NULL, NULL);

//         // Present the renderer
//         SDL_RenderPresent(renderer);
//     }

//     // Clean up
//     SDL_DestroyTexture(texture);
//     closeSDL();
//     return 0;
// }
