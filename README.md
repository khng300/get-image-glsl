# get-image-glsl

This program renders a fragment shader, it aims at handlging any version
of GLSL, including GLSL ES.

# TODO

 - init glfw

 - factorize all egl variables (EGLDisplay, etc) into a single
   struct. idem for glfw

# Developper notes / design choices

For any error, use the crash() macro defined in `common.h`. Do not
hesitate to crash as soon as something goes wrong, rather than passing
around return values.
