# get-image-glsl

This program renders a fragment shader, it can be compiled with
different context provider, currently EGL or GLFW.

# Usage

Note: For an up-to-date option overview, just run one of the executables
without any arguments, it should list its options.

```
Usage: ./get_image_<egl/glfw> [options] shader.frag

The program will look for a JSON whose name is derived from the
shader as '<shader>.json'. This JSON file can contain uniforms
initialisations. If no JSON file is found, the program uses default
values for some uniforms.

Options are:
  --delay                            number of frames before PNG capture
  --exit-compile                     exit after compilation
  --exit-linking                     exit after linking
  --output file.png                  set PNG output file name
  --resolution <width> <height>      set resolution, in Pixels
  --vertex shader.vert               use a specific vertex shader
  --dump_bin <file>                  dump binary output to given file

Return values:
  0    Successful rendering
  1    Error
  101  Shader compilation error (either fragment or vertex)
  102  Shader linking error
```

# Developer notes

## TODO

- add option to force a particular opengl api version (with glfw)

## Crash early!

For any error, use the `crash()` macro defined in `common.h`. Do not
hesitate to crash as soon as something goes wrong, rather than passing
around return values.
