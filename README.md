# get-image-glsl

This program renders a fragment shader, it can be compiled with
different context provider, currently EGL or GLFW.

# Usage

Note: For an up-to-date option overview, just run one of the executables
without any arguments, it should list its options.

```
Usage: ./get_image_<egl/glfw> [options] shader.frag
Options are:
  --exit-compile                     exit after compilation
  --exit-linking                     exit after linking
  --output file.png                  set PNG output file name
  --resolution <width> <height>      set resolution, in Pixels
  --vertex shader.vert               use a specific vertex shader
```

# Developer notes

## TODO

- raise specific error codes for some failures

## Crash early!

For any error, use the crash() macro defined in `common.h`. Do not
hesitate to crash as soon as something goes wrong, rather than passing
around return values.
