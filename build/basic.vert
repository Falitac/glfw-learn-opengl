#version 400 core
uniform mat4 MVP;

in vec3 vCol;
in vec2 vPos;
in vec2 vUV;

out vec2 uv;
out vec3 color;

void main()
{
  gl_Position = MVP*vec4(vPos, 0.0, 1.0);
  color = vCol;
  uv = vUV;
}