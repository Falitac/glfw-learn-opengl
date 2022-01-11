#pragma once

#include <string>
#include <GL/glew.h>

class Texture {
public:
  GLuint texture;
  int width, height, channels;

  void gen(const std::string& localisation);
  void destruct();
};