#include "mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

void Mesh::gen() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(
    GL_ARRAY_BUFFER,
    vertices.size() * sizeof(Vertex),
    vertices.data(),
     GL_STATIC_DRAW
  );  

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    indices.size() * sizeof(unsigned int),
    indices.data(),
    GL_STATIC_DRAW
  );

  // vertex positions
  glEnableVertexAttribArray(0);	
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  // vertex normals
  glEnableVertexAttribArray(1);	
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);	
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

  glBindVertexArray(0);
}

void Mesh::load(const std::string& location) {

  Assimp::Importer importer;
  const auto scene = importer.ReadFile(location, aiProcess_Triangulate | aiProcess_FlipUVs);

  if(scene->HasMaterials()) {
    printf("Materials found!\n");
  }

  for(auto i = 0; i < scene->mNumMeshes; i++) {
    printf("%llu mesh found\n", i);

    auto m = scene->mMeshes[i];
    vertices.reserve(m->mNumVertices);

    for(auto j = 0; j < m->mNumVertices; j++) {
      Vertex v;

      v.position.x = m->mVertices[j].x;
      v.position.y = m->mVertices[j].y;
      v.position.z = m->mVertices[j].z;

      v.normal.x = m->mNormals[j].x;
      v.normal.y = m->mNormals[j].y;
      v.normal.z = m->mNormals[j].z;

      //m->mNumUVComponents;
      v.uv.x = m->mTextureCoords[0][j].x;
      v.uv.y = m->mTextureCoords[0][j].y;

      vertices.push_back(std::move(v));
    }
    indices.reserve(m->mNumFaces);
    for(auto j = 0; j < m->mNumFaces; j++) {
      auto face = m->mFaces[j];
      for(auto k = 0; k < face.mNumIndices; k++) {
        indices.emplace_back(face.mIndices[k]);
      }
    }
  }
  gen();

}

void Mesh::render(Shader& shader) {
  glBindVertexArray(VAO);
  shader.use();
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}