#pragma once

#include <glm/glm.hpp>
#include "Mesh.hpp"

class Player
{
private:
 glm::vec2 position;
 glm::vec2 velocity;

public:
  Mesh model;
};
