#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

#include "shader.hpp"

struct Vertex {
  glm::vec3 position;
  glm::vec2 uv;
  glm::vec3 normal;
};

class Mesh {
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  unsigned int VAO, VBO, EBO;
public:
  void gen();
  void load(const std::string& location);

  void render(Shader& shader);

};