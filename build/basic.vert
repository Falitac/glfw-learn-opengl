#version 400 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

uniform mat4 MVP;
uniform mat4 Model;
uniform float time;
out vec2 uv;

out float lightValue;

void main() {
  gl_Position = MVP*vec4(vPos, 1.0);

  vec3 modelPos = vec3(Model*vec4(vPos, 1.0));
  vec3 normal = vec3(Model *vec4(vNormal, 1.0));
  normal = normalize(normal);

  float radius = 7.0;
  vec3 lightPosition;
  lightPosition.x = radius * sin(time);
  lightPosition.z = radius * cos(time);
  //float radius2 = length(lightPosition.xy);
  //lightPosition.z = radius2 * sin(time);

  //mat3 Normal = mat3(transpose(inverse(Model))) * vNormal;
  

  vec3 lightDirection = vec3(0.0, 1.0, 0.0);
  lightDirection = normalize(lightDirection - modelPos);
  lightValue = dot(normalize(normal), lightDirection);
  lightValue = clamp(lightValue, 0.3, 1.0);

  uv = vUV;
}