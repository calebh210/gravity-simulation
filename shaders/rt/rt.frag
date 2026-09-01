#version 430 core
in vec2 TexCoord;
out vec4 frag_colour;

uniform sampler2D screenTex;

void main() {
    frag_colour = texture(screenTex, TexCoord);
}