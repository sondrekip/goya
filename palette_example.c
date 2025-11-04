/*
 * Example: How to use the palette system
 * 
 * This demonstrates the workflow:
 * 1. Create/load a palette
 * 2. Generate images in UVW color space (your existing code)
 * 3. Convert UVW → Paletted (8-bit indices)
 * 4. Convert Paletted → RGB (for SDL display)
 */

#include "palette.h"
#include "render.h"
#include <stdio.h>

void example_palette_usage() {
    // Step 1: Create a palette with bit allocation
    // Example: 5 bits for U, 3 bits for V, 0 bits for W
    palette my_palette;
    my_palette.u_bits = 5;
    my_palette.v_bits = 3;
    my_palette.w_bits = 0;  // Must sum to 8
    
    // Step 2: Load palette colors from a PNG file
    // The PNG should be 256x1 pixels (or 16x16 pixels)
    // Each pixel's RGB becomes a palette entry (R→U, G→V, B→W)
    if (!load_palette_from_png("palette.png", &my_palette)) {
        fprintf(stderr, "Failed to load palette, using default\n");
        // You could manually populate my_palette.colours[256] here
    }
    
    // Step 3: Generate your image in UVW color space (your existing code)
    // base_layer is already in UVW format
    colour uvw_buffer[WIDTH * HEIGHT];
    // ... fill uvw_buffer with your UVW colors ...
    
    // Step 4: Convert UVW image to paletted (8-bit indices)
    uint8_t paletted_buffer[WIDTH * HEIGHT];
    convert_uvw_to_paletted(uvw_buffer, paletted_buffer, &my_palette, WIDTH, HEIGHT);
    
    // Step 5: Convert paletted image back to RGB for SDL display
    colour rgb_buffer[WIDTH * HEIGHT];
    convert_paletted_to_rgb(paletted_buffer, rgb_buffer, &my_palette, WIDTH, HEIGHT);
    
    // Step 6: Use rgb_buffer with SDL (same as you currently use base_layer)
    // base_layer = rgb_buffer; // or copy it
}

/*
 * Alternative: Use a manually created palette
 */
void example_manual_palette() {
    palette my_palette;
    my_palette.u_bits = 5;
    my_palette.v_bits = 3;
    my_palette.w_bits = 0;
    
    // Manually create palette entries
    // For example, create a gradient palette
    for (int i = 0; i < 256; i++) {
        my_palette.colours[i].u = i;  // Red component
        my_palette.colours[i].v = (i * 2) % 256;  // Green component
        my_palette.colours[i].w = (255 - i);  // Blue component
        my_palette.colours[i].a = 255;  // Full opacity
    }
    
    // Now use it as above
    // convert_uvw_to_paletted(...);
    // convert_paletted_to_rgb(...);
}

/*
 * Integration with existing code:
 * 
 * In your main.c, you could do:
 * 
 * 1. After init_SDL(), create and load palette
 * 2. After generating base_layer (in UVW), convert to paletted
 * 3. Convert paletted to RGB and update base_layer
 * 4. Render as normal
 */
