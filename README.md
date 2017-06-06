# get-image-glsl

This program renders a fragment shader, it can be compiled with
different context provider, currently EGL or GLFW.

# Crash early!

For any error, use the crash() macro defined in `common.h`. Do not
hesitate to crash as soon as something goes wrong, rather than passing
around return values.

# TODO

- raise specific error codes for some failures
