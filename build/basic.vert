#version 400 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

uniform mat4 MVP;
out vec2 uv;

out float lightValue;

void main()
{
  gl_Position = MVP*vec4(vPos, 1.0);
  uv = vUV;

  vec3 lightDirection = vec3(0.0, 0.0, 1.0);
  lightValue = dot(vNormal, lightDirection);
  lightValue = clamp(lightValue, 0.2, 1.0);
}