#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Plane
{
private:
  GLuint VAO, VBO, EBO;
public:
  Plane(float width, float height, glm::vec3 position);
};

