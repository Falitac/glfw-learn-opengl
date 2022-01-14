#pragma once
#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <array>

#include "Shader.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "AssetManager.hpp"

class App {
public:
	App();

	void run();

  static constexpr double dt = 1.0 / 60.0;

private:
	void init();
	void otherInit();
	void cleanup();

	void loop();

	void update();
	void render();

	friend void glErrorCallback(int, const char*);
	friend void keyCallback(GLFWwindow*, int, int, int, int);
	friend void windowResizeCallback(GLFWwindow*, int, int);

private:
	bool isRunning = true;
  GLFWwindow* window;
  int width, height;
  float aspectRatio;
  Camera camera;

  std::map<int, bool> pressedKeys;

	AssetManager assetManager;
  Shader s;

  GLuint texture1;

  Mesh playerMesh;
  glm::vec3 playerPosition;
  glm::vec3 playerVelocity;
  float playerDirectionAngle = 0.0f;
  float playerChangeAngle = 0.0f;

  Mesh bombMesh;
  std::vector<glm::vec3> bombPositions;

  Mesh cube;
  std::vector<std::vector<int>> grid;

  Mesh floor;

  Texture texture;
  Texture flower;

  double scale = 1.0;
  bool projectionSwitch = false;
  bool reloadShader = false;

  int frameCounter = 0;
  int tickCounter = 0;

  std::array<glm::vec2, 10> mouseMoveHistory;
};
