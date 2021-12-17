#include "shader.hpp"

#include <string.h>
#include <iostream>

Shader::Shader() {

}

Shader::~Shader() {

}


std::string Shader::loadFromFileContent(const std::string& fileName) {
  std::ifstream file(fileName);
  std::stringstream s;

  if(file.good()) {
    s << file.rdbuf();
  } else {
    throw FileNotFound();
  }
  return s.str();
}

void Shader::compile(const std::string& shaderName) {
  programID = glCreateProgram();
  try {
    auto vertexShader = compileShader(shaderName, GL_VERTEX_SHADER);
    glAttachShader(programID, vertexShader);
    glDeleteShader(vertexShader);
  } catch(FileNotFound& e) {
    printf("%s\n", e.what());
  }
  try {
    auto fragmentShader = compileShader(shaderName, GL_FRAGMENT_SHADER);
    glAttachShader(programID, fragmentShader);
    glDeleteShader(fragmentShader);
  } catch(FileNotFound& e) {
    printf("%s\n", e.what());
  }
  try {
    auto geometryShader = compileShader(shaderName, GL_GEOMETRY_SHADER);
    glAttachShader(programID, geometryShader);
    glDeleteShader(geometryShader);
  } catch(FileNotFound& e) {
    printf("%s\n", e.what());
  }

  glLinkProgram(programID);

  GLint compilationResult, errorLogSize;
  glGetProgramiv(programID, GL_LINK_STATUS, &compilationResult);
  glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &errorLogSize);

  if(errorLogSize > 0) {
    std::string errorMessage;
    errorMessage.reserve(errorLogSize);
    glGetProgramInfoLog(programID, errorMessage.size(), NULL, errorMessage.data());
    fprintf(stderr, "Error compiling program %s\n", errorMessage.c_str());
  }


}

GLuint Shader::compileShader(const std::string& shaderName, GLenum shaderType) {
  static std::string EXTENSIONS[] = {".vert", ".frag", ".geom"};
  std::size_t fileExtensionID = 0;

  switch (shaderType) {
    case GL_VERTEX_SHADER:   fileExtensionID = 0; break;
    case GL_FRAGMENT_SHADER: fileExtensionID = 1; break;
    case GL_GEOMETRY_SHADER: fileExtensionID = 2; break;
    default: throw WrongShaderType(); break;
  }
  auto shaderSource = loadFromFileContent(shaderName + EXTENSIONS[fileExtensionID]);
  const char* shaderSourceCasted  = shaderSource.c_str();

  auto resultShader = glCreateShader(shaderType);
  glShaderSource(resultShader, 1, &shaderSourceCasted, NULL);
  glCompileShader(resultShader);

  GLint compilationResult, errorLogSize;
  glGetShaderiv(resultShader, GL_COMPILE_STATUS, &compilationResult);
  glGetShaderiv(resultShader, GL_INFO_LOG_LENGTH, &errorLogSize);

  if(errorLogSize > 0) {
    std::string errorMessage;
    errorMessage.reserve(errorLogSize);
    glGetShaderInfoLog(resultShader, errorMessage.size(), NULL, &errorMessage[0]);

    fprintf(stderr, "Compiling %s error:\n", (shaderName + EXTENSIONS[fileExtensionID]).c_str());
    fprintf(stderr, "%s\n", errorMessage.c_str());
  }


  return resultShader;
}

GLint Shader::findVarLocation(const std::string& varName) {
  if(!variableLocations.contains(varName)) {
    auto id = glGetAttribLocation(programID, varName.c_str());
    variableLocations[varName] = id;
  }
  return variableLocations[varName];
}

GLint  Shader::findUniformLocation(const std::string& uniformName) {
  if(!uniformLocations.contains(uniformName)) {
    auto id = glGetAttribLocation(programID, uniformName.c_str());
    variableLocations[uniformName] = id;
  }
  return variableLocations[uniformName];
}

void Shader::use() {
  glUseProgram(programID);
}