#pragma once

#include <glm/glm.hpp>

class Camera
{
private:
  glm::vec3 position;
  float horizontalAngle;
  float verticalAngle;
public:
  Camera();
};

Camera::Camera()
{
}

