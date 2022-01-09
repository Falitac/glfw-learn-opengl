#pragma once

#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <exception>

class Shader
{
private:
  GLuint programID;
  std::map<std::string, GLuint> variableLocations;
  std::map<std::string, GLuint> uniformLocations;
public:
  Shader() = default;
  Shader(const std::string& shaderName);

  GLuint getProgramID() const {
    return programID;
  }

  void use();
  GLint findVarLocation(const std::string& varName);
  GLint findUniformLocation(const std::string& uniformName);

private:
  std::string loadFromFileContent(const std::string& shaderName);
  void compile(const std::string& shaderName);
  GLuint compileShader(const std::string& shaderName, GLenum shaderType);
  friend int main(void);

  class FileNotFound : public std::exception {
    public:
    virtual const char* what() {
      return "That file does not exist!";
    }
  };
  class WrongShaderType : public std::exception {
    public:
    virtual const char* what() {
      return "Wrong shader type!";
    }
  };
};


