#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

#include "shader.hpp"

struct Vertex {
  glm::vec3 position;
  glm::vec2 uv;
  glm::vec2 normal;

};

class Mesh {
  std::vector<Vertex> vertices;
public:

  void render(Shader& shader);

};