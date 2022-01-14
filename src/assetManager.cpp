#include "AssetManager.hpp"

void AssetManager::addTexture(const std::string& location, const std::string& textureName) {
  auto entry = std::make_pair(location, Texture());
  entry.second.gen(location);
  textures.insert(std::move(entry));
}

void AssetManager::addMesh(const std::string& location, const std::string& meshName) {
  auto entry = std::make_pair(location, Mesh());
  entry.second.load(location);
  meshes.insert(std::move(entry));
}

void AssetManager::addShader(const std::string& location, const std::string& shaderName) {
  auto entry = std::make_pair(location, Shader(shaderName));
  shaders.insert(std::move(entry));
}

Texture& AssetManager::getTexture(const std::string& name) {
  return textures.at(name);
}

Mesh& AssetManager::getMesh(const std::string& name) {
  return meshes.at(name);
}

Shader& AssetManager::getShader(const std::string& name) {
  return shaders.at(name);
}
