
# Define the compiler and flags
cc = clang
cflags = -I/usr/local/include/SDL2 -oz -DSDL_PLATFORM
ldflags = -L/usr/local/lib -lSDL2

# Define the target executable
target = vector_draw

# Define the source files
# Core palette functions (platform-agnostic)
# SDL-specific palette loading (only for Mac/development)
sources = main.c render.c display.c palette.c palette_png.c palette_sdl.c

# Define the object files (derived from source files)
objects = $(sources:.c=.o)

# Default target (what 'make' runs by default)
all: $(target)

# Rule to build the target executable
$(target): $(objects)
	$(cc) -o $(target) $(objects) $(ldflags)

# Rule to compile source files into object files
%.o: %.c
	$(cc) $(cflags) -c $< -o $@

# Rule to clean up the build (remove object files and the executable)
clean:
	rm -f $(objects) $(target)

# Phony targets (not real files)
.PHONY: all clean
