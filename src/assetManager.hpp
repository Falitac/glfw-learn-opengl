#pragma once

#include "shader.hpp"
#include "texture.hpp"
#include <map>
#include <string>

class AssetManager {
public:
  Shader basic;
  std::map<std::string, Texture> textures;
  std::vector<Mesh> meshes;

};