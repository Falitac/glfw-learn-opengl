#version 400 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

uniform mat4 MVP;
uniform mat4 Model;
uniform float time;

out vec2 uv;
out vec3 fNormal;
out vec3 fPos;


void main() {
  gl_Position = MVP * vec4(vPos, 1.0);

  fPos = vec3(Model * vec4(vPos, 1.0));
  uv = vUV;
  fNormal = mat3(transpose(inverse(Model))) * vNormal;  
  fNormal = normalize(fNormal);
}