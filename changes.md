Summary of Changes
1. Fixed palette functions (palette.c and palette.h)
Fixed uvwa_to_index(): corrected bit packing. Extracts the upper N bits from each UVW component and packs them into an 8-bit index:
U bits go in the highest positions
V bits go in the middle
W bits go in the lowest positions
Example: if u_bits=5, v_bits=3, w_bits=0, the layout is [uuuuu][vvv]
Renamed index_to_rgba() to index_to_colour() and fixed it to return a colour struct instead of a broken cast.
2. Added palette loading from PNG (palette.c)
Added load_palette_from_png() that:
Loads a PNG palette file (256x1 pixels or 16x16 pixels)
Reads RGB values from each pixel
Stores them in the palette's colours[256] array
Treats RGB as UVW axes (R→U, G→V, B→W)
3. Created conversion layers (palette.c)
convert_uvw_to_paletted(): converts a UVW color buffer to an 8-bit paletted image by calling uvwa_to_index() for each pixel
convert_paletted_to_rgb(): converts a paletted image (8-bit indices) back to RGB colors using the palette lookup
4. Updated build system
Updated Makefile to:
Include palette.c in the build
Link against SDL2_image for PNG loading
Updated display.c to:
Initialize SDL_image with IMG_Init()
Clean up with IMG_Quit() on shutdown
How It Works
Generate images in UVW color space (existing code)
Convert UVW → paletted: use convert_uvw_to_paletted() with a palette configured with bit allocation (e.g., u_bits=5, v_bits=3, w_bits=0)
Convert paletted → RGB: use convert_paletted_to_rgb() to get RGB colors for SDL display
Render: send the RGB buffer to SDL as before
You can load a palette from a PNG file (256x1 or 16x16) using load_palette_from_png(), configure the bit allocation in the palette struct, and use the conversion functions to transform between UVW and paletted formats.
