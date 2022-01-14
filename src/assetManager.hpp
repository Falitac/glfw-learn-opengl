#pragma once

#include "Shader.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include <map>
#include <vector>
#include <string>

class AssetManager {
  Shader basic;
  std::map<std::string, Shader> shaders;
  std::map<std::string, Texture> textures;
  std::map<std::string, Mesh> meshes;
public:

  void addTexture(const std::string& location, const std::string& textureName = "");
  void addMesh(const std::string& location, const std::string& meshName = "");
  void addShader(const std::string& location, const std::string& shaderName = "");

  Texture& getTexture(const std::string& name);
  Mesh& getMesh(const std::string& name);
  Shader& getShader(const std::string& name);

};