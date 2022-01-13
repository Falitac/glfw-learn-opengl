#include "Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>


void Texture::gen(const std::string& localisation) {
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  auto img_data = stbi_load(localisation.c_str(), &width, &height, &channels, 0);
  if(!img_data) {
    std::perror("Couldn't load image data");
    return;
  }
  std::printf("IMG_WIDTH: %d\n", width);
  std::printf("IMG_HEIGHT: %d\n", height);
  std::printf("IMG_CHANNELS: %d\n", channels);

  int deliberateChannels = GL_RGB;
  switch (channels)
  {
  case 4:
    deliberateChannels = GL_RGBA;
  break;
  
  default:
    deliberateChannels = GL_RGB;
    break;
  }
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    deliberateChannels,
    width,
    height,
    0,
    deliberateChannels,
    GL_UNSIGNED_BYTE,
    img_data
  );
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(img_data);
}

void Texture::destruct() {
  glDeleteTextures(1, &texture);
}