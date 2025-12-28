#version 460 core
uniform vec3 objectColor;
out vec4 frag_colour;
void main() {
  frag_colour = vec4( objectColor, 1.0 );
}