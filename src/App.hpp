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
#include <tuple>

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
	friend void scrollCallback(GLFWwindow*, double, double);

  friend void mouseButtonCallback(GLFWwindow*, int, int, int);


private:
  GLFWmonitor** monitors = nullptr;
  int monitorCount = 0;

	bool isRunning = true;
	bool isFullScreen = false;
  GLFWwindow* window;
  int width, height;
  float aspectRatio;
  Camera camera;
  float camRadius = 7.f;
  float camAngle = 0.4f * glm::half_pi<float>();

  std::map<int, bool> pressedKeys;
  glm::vec2 mouseDiff;

	AssetManager assetManager;
  Shader s;

  GLuint texture1;

  Mesh playerMesh;
  glm::vec3 playerPosition;
  glm::vec3 playerVelocity;
  float playerDirectionAngle = 0.0f;
  float playerChangeAngle = 0.0f;
  int bombAmount = 200;
  float bombDestructionTime = 2.0f;
  float bombCooldown = 0.08f;
  float bombCooldownTimer = 0.0f;
  int bombPower = 6;

  Mesh bombMesh;
  std::vector<std::tuple<glm::vec3, float>> bombs;

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
