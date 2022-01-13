#version 400 core

in vec2 uv;
in vec3 fNormal;
in vec3 fPos;

out vec4 resultColor;

uniform sampler2D texture1;
uniform float time;
uniform vec3 playerPos;

vec3 calculateDiffuse(vec3 lightPosition, float lightPower = 100.0, vec3 lightColor = vec3(1.0)) {
  vec3 vecDifference = lightPosition - fPos;
  float squaredDistance = dot(vecDifference, vecDifference);

  vec3 lightDirection = normalize(vecDifference);
  float diffuseValue = dot(fNormal, lightDirection);
  diffuseValue = clamp(diffuseValue, 0.0, 1.0);

  return lightColor * diffuseValue / squaredDistance * lightPower;
}

void main() {
  vec3 objectColor = texture(texture1, uv).xyz;
  vec3 ambient = vec3(0.22);
  vec3 diffuse = vec3(0.0);

  vec3 colors[9];
  colors[0] = vec3(0.0, 0.0, 1.0);
  colors[1] = vec3(0.0, 0.0, 0.0);
  colors[2] = vec3(0.0, 1.0, 1.0);
  colors[3] = vec3(0.0, 1.0, 0.0);
  colors[4] = vec3(1.0, 0.0, 1.0);
  colors[5] = vec3(1.0, 0.0, 0.0);
  colors[6] = vec3(1.0, 1.0, 1.0);
  colors[7] = vec3(1.0, 1.0, 0.0);
  colors[8] = vec3(0.0, 0.0, 1.0);

  float stride = 16.0;
  float offset = 1.0;
  vec3 lightPos;
  lightPos.y = 20.0;
  for(int x = 0; x < 3; x++) {
    for(int z = 0; z < 3; z++) {
      lightPos.x = stride * float(x) + offset;
      lightPos.z = 2 * stride * float(z) + offset;
      vec3 color = colors[x * 3 + z];
      diffuse += calculateDiffuse(lightPos, 200.0, color);
    }
  }


  resultColor.xyz = (ambient + diffuse) * objectColor;
  //resultColor.xyz = objectColor;
}