#pragma once

#include <GL/glew.h>
#include <string>

class Texture {
public:
  GLuint texture;
  int width, height, channels;

  void gen(const std::string& localisation);
  void destruct();
};