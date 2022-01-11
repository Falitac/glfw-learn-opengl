#pragma once

#include <glm/glm.hpp>
#include "mesh.hpp"

class Player
{
private:
 glm::vec2 position;
 glm::vec2 velocity;

public:
  Mesh model;
};
