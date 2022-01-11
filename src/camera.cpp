#include "camera.hpp"


Camera::Camera(const glm::vec3& initPosition)
    : position(initPosition)
    , fieldOfView(48.f)
    , horizontalAngle(0.f)
    , verticalAngle(0.f)
{

}

void Camera::update(double dt) {

  camSpeed *= dt;
  strafeSpeed *= dt;
  horizontalAngle += glm::radians(rotateHor) * dt;
  verticalAngle += glm::radians(rotateVert) * dt;
  move(dir() * camSpeed + right() * strafeSpeed);
    direction = glm::vec3(
        std::cos(verticalAngle) * std::sin(horizontalAngle),
        std::sin(verticalAngle),
        std::cos(verticalAngle) * std::cos(horizontalAngle)
    );
    rightDirection = glm::vec3(
        sin(horizontalAngle - glm::half_pi<float>()),
        0,
        cos(horizontalAngle - glm::half_pi<float>())
    );
    upDirection = glm::cross(rightDirection, direction);
    resetChanges();
}


void Camera::resetChanges() {
  camSpeed = 0.f;
  strafeSpeed = 0.f;
  rotateHor = 0.0f;
  rotateVert = 0.0f;
  maxSpeed = 1000.0f;
}