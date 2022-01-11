#pragma once

#include <glm/glm.hpp>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


class Camera
{
public:
  Camera(const glm::vec3& initPosition = glm::vec3(0.f, 0.f, 0.f));


  inline float& FOV()       { return fieldOfView;     };
  inline float& vertAngle() { return verticalAngle;   };
  inline float& horAngle()  { return horizontalAngle; };
  inline glm::vec3& pos()   { return position;        };

  const inline glm::vec3& dir()   const { return direction;      };
  const inline glm::vec3& right() const { return rightDirection; };
  const inline glm::vec3& up()    const { return upDirection;    };

  inline void setPosition(const glm::vec3& where) {
      position = where;
  }

  inline void move(const glm::vec3& deltaVector) {
      position += deltaVector;
  }

  inline glm::mat4 view() {
      return glm::lookAt(position, position + direction, upDirection);
  }

  void update(double dt);

  float camSpeed = 0.f;
  float strafeSpeed = 0.f;
  float rotateHor = 0.0f;
  float rotateVert = 0.0f;
  float maxSpeed = 1000.0f;
private:
  float fieldOfView;
  float verticalAngle;
  float horizontalAngle;

  glm::vec3 direction;
  glm::vec3 rightDirection;
  glm::vec3 upDirection;

  glm::vec3 position;

  void resetChanges();
};