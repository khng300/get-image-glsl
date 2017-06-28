# get-image-glsl

This program renders a fragment shader, it can be compiled with
different context provider, currently EGL or GLFW. See build
instructions below.

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

# Build

## Linux

Requirement: `cmake`. Use this script to build the glfw version (the
script automatically download and install the glfw version for the
build):

```
cd build
./build-linux
```

Alternatively, if you already have glfw3 and egl installed, you can use
the Makefile (just call `make`).

## Windows

Only GLFW build is supported, on Linux host. Use this script:

```
cd build
./build-x86_64-w64-mingw32
```

# Developer notes

## TODO

- add option to force a particular opengl api version (with glfw)

- fix cmake scripts for EGL compilation

- add CI

## Crash early!

For any error, use the `crash()` macro defined in `common.h`. Do not
hesitate to crash as soon as something goes wrong, rather than passing
around return values.
