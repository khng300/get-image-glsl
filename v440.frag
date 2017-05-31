#version 440

out vec4 _GLF_Color;

void main(void) {
  _GLF_Color = vec4(gl_FragCoord.x/256.0, gl_FragCoord.y/256.0, 1.0, 1.0);
}
