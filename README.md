# get-image-glsl

This program renders a fragment shader, it aims at handlging any version
of GLSL, including GLSL ES.

# Developper notes / design choices

For any error, use the crash() macro. Do not hesitate to crash as soon
as something goes wrong, rather than passing around return values.
