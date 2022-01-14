#version 400 core

in vec2 uv;
in vec3 fNormal;
in vec3 fPos;

out vec4 resultColor;

uniform sampler2D texture1;
uniform float time;
uniform vec3 playerPos;
uniform vec3 camPos;

vec3 calculateDiffuse(vec3 lightPosition, float lightPower = 100.0, vec3 lightColor = vec3(1.0)) {
  vec3 vecDifference = lightPosition - fPos;

  vec3 lightDirection = normalize(vecDifference);
  float diffuseValue = dot(fNormal, lightDirection);
  diffuseValue = clamp(diffuseValue, 0.0, 1.0);

  return lightColor * diffuseValue * lightPower;
}

vec3 calculateSpecular(vec3 lightPosition, float strength = 0.5, vec3 lightColor = vec3(1.0)) {
  vec3 lightDirection = normalize(lightPosition - fPos);
  vec3 viewDirection = normalize(camPos - fPos);

  vec3 reflectDirection = reflect(-lightDirection, fNormal);

  float specularity = pow(max(dot(reflectDirection, viewDirection), 0.0), 256);

  return strength * specularity * lightColor;
}

void main() {
  vec3 objectColor = texture(texture1, uv).xyz;
  vec3 ambient = vec3(0.22);
  vec3 diffuse = vec3(0.0);
  vec3 specular = vec3(0.0);

  vec3 colors[9];
  colors[0] = vec3(0.0, 0.0, 1.0);
  colors[1] = vec3(0.0, 0.0, 0.0);
  colors[2] = vec3(0.0, 1.0, 1.0);
  colors[3] = vec3(0.0, 1.0, 0.0);
  colors[4] = vec3(1.0, 0.0, 1.0);
  colors[5] = vec3(1.0, 0.0, 0.0);
  colors[6] = vec3(1.0, 1.0, 1.0);
  colors[7] = vec3(1.0, 1.0, 0.0);
  colors[8] = vec3(sin(time) * sin(time), sin(time), cos(time));

  float stride = 16.0;
  float offset = 4.0;
  vec3 lightPos;
  lightPos.y = 10.0;
  for(int x = 0; x < 3; x++) {
    for(int z = 0; z < 3; z++) {
      lightPos.x = 0.8 * stride * float(x) + 1.0;
      lightPos.z = 2.0 * stride * float(z) + offset;
      vec3 color = colors[x * 3 + z];

      vec3 vecDifference = lightPos - fPos;
      float squaredDistance = dot(vecDifference, vecDifference);

      diffuse += calculateDiffuse(lightPos, 100.0, color) / squaredDistance;

      specular += calculateSpecular(lightPos, 6.7, color * 1.2);
    }
  }

  resultColor.xyz = (ambient + diffuse + specular) * objectColor;
  //resultColor.xyz = objectColor;
}