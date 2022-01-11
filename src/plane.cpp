#include "plane.hpp"

#include <vector>

Plane::Plane(float width, float height, glm::vec3 position) {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  std::vector<glm::vec3> vertices =
  {
    {0.0f, 0.0f,  0.0f},
    {1.0f, 0.0f, -1.0f},
    {0.0f, 0.0f, -1.0f},
    {1.0f, 0.0f,  0.0f}
  };
  std::vector<unsigned int> indices = {
    0, 1, 2,
    0, 3, 1
  };
  
  glBindVertexArray(VAO);
  glBindVertexArray(VBO);
  glBufferData(
    GL_ARRAY_BUFFER,
    vertices.size() * sizeof(glm::vec3),
    vertices.data(),
    GL_STATIC_DRAW
  );

  glEnableVertexAttribArray(0);	
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

  glEnableVertexAttribArray(0);	
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

  glBindVertexArray(0);

}