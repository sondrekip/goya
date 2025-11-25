
# Define the compiler and flags
cc = clang
cflags = -I/usr/local/include/SDL2 -oz
ldflags = -L/usr/local/lib -lSDL2

common_sources = render.c display.c palette.c genpal.c
common_objects = $(common_sources:.c=.o)

vector_draw = vector_draw
vector_draw_sources = main.c
vector_draw_objects = $(vector_draw_sources:.c=.o)

testpal = testpal
testpal_sources = testpal.c
testpal_objects = $(testpal_sources:.c=.o)


# Default target (what 'make' runs by default)
all: $(vector_draw) $(testpal)

$(vector_draw): $(common_objects) $(vector_draw_objects)
	$(cc) -o $@ $^ $(ldflags)

$(testpal): $(common_objects) $(testpal_objects)
	$(cc) -o $@ $^ $(ldflags)

# Rule to compile source files into object files
%.o: %.c
	$(cc) $(cflags) -c $< -o $@

# Rule to clean up the build (remove object files and the executable)
clean:
	rm -f $(vector_draw) $(testpal)

# Phony targets (not real files)
.PHONY: all clean
