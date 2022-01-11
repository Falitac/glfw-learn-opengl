#pragma once

#include "shader.hpp"
#include "texture.hpp"
#include "Mesh.hpp"
#include <map>
#include <vector>
#include <string>

class AssetManager {
public:
  Shader basic;
  std::map<std::string, Texture> textures;
  std::vector<Mesh> meshes;

};