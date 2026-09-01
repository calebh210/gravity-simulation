#version 430 core
layout(location = 0) in vec2 vp;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
out uvec2 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
//   gl_Position = projection * view * model * vec4( vp, 0.0, 1.0 );
  gl_Position = vec4( vp, 0.0, 1.0 );
  TexCoord = aTexCoord;
}