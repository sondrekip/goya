
typedef struct palette {
	unsigned char u_bits, v_bits, w_bits;
	colour colours[256];
} palette;

char uvwa_to_index(
    colour uvwa, 
    palette pal);
uint32_t index_to_rgba(
    char index, 
    palette pal);
