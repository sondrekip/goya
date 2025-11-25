# todo

- generate indexed image from uvwa image
- combine palette and indexed image to rgb image

I think this looks like a good overview. I came to think of a couple of other concerns I have that it would be good to keep in mind for both of us.

- I would like to make the code more modular and easy to understand
- Specifically I want to keep the generation up to the indexed image separate from the sdl display code. At some point I want to copy the image generation code to an amiga to make it run there, and naturally the sdl part will be left out.
- Defining the base_layer as a globally referenced array seems like something that could be done better. Perhaps I could instead declare the array in main and pass its pointer to the other functions.
- I am also concerned about all the arguments to functions in render.c that are just passed on to other functions. This seems unnecessarily complicated and makes the code hard to understand.

- type for uvwa_buffer that includes height and width
- compile for amiga on vscode
- adjust colour range for translucency
- make downscaling from 640x512 to 320x256
- make anti-aliased poly drawing
- make dithering
- make line drawing
- make prime shape drawing (circle, half circle)
- make spline drawing
- make relative vector positions and scaling
