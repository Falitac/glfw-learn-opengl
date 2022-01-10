#version 400 core
in vec3 color;
in vec2 uv;
out vec4 resultColor;

uniform sampler2D texture1;

void main()
{
  vec3 mixed = mix(color, texture(texture1, uv).xyz, 0.9);
  resultColor = vec4(mixed, 1.0);
}