#version 400 core

in vec2 uv;
in float lightValue;

out vec4 resultColor;

uniform sampler2D texture1;

void main()
{
  vec3 mixed = mix(vec3(1.0, 0.0, 0.0), texture(texture1, uv).xyz, 0.9);
  //resultColor = vec4(mixed, 1.0);
  resultColor = texture(texture1, uv) * lightValue;
}