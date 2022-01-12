#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>


#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <array>


#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Context {
  GLFWwindow* window;
  int width, height;
  float aspectRatio;
  Camera camera;

  std::map<int, bool> pressedKeys;
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
  const double tickTime = 1.0 / 60.0;

  std::array<glm::vec2, 10> mouseMoveHistory;

} context;

static void errorCallback(int error, const char* description);
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void windowResizeCallback(GLFWwindow* window, int width, int height);
static void init();
static void update(double dt);
static void draw();

int main(void)
{
  auto glErrorChecker = [](int line) {
    static int errorCount;
    GLenum glError;
    while((glError = glGetError()) != GL_NO_ERROR) {
      printf("Line: %d\n", line);
      printf("%0x\n", glError);
      errorCount++;
    }
  };

  init();

  context.s = {"basic"};

  context.playerMesh.load("assets/obj/robo-boodie.obj");
  context.bombMesh.load("assets/obj/bomb.obj");
  context.floor.load("assets/obj/plane.obj");
  context.cube.load("assets/obj/cube.obj");

  context.texture.gen("assets/textures/low-poly.png");
  context.flower.gen("assets/textures/flower-texture.jpg");

  context.camera.pos() = {10.0f, 12.0f, 0.0f};
  context.camera.vertAngle() = -glm::quarter_pi<float>();
  context.camera.horAngle() = -glm::half_pi<float>();

  context.playerPosition = {0.0f, 0.0f, 0.0f};

  context.grid = std::vector(16, std::vector<int>(33, 0));

  for(auto it = context.grid.begin(); it != context.grid.end(); it++) {
    if(it == context.grid.begin() || it + 1 == context.grid.end()) {
      for(auto& element : *it) {
        element = 1;
      }
      continue;
    }
    it->front() = 1;
    it->back() = 1;
  }

  double timeAccumulator = 0.0;
  double lastFrameTime = 0.0;
  double fpsClock = 0.0f;

  while (!glfwWindowShouldClose(context.window)) {
    double frameStartTime = glfwGetTime();
    double frameTimeDifference = frameStartTime - lastFrameTime;
    lastFrameTime = frameStartTime;
    timeAccumulator += frameTimeDifference;

    if(context.reloadShader) {
      context.reloadShader = false;
      glDeleteProgram(context.s());
      context.s = {"basic"};
    }

    while(timeAccumulator > context.tickTime) {
      context.tickCounter++;
      timeAccumulator -= context.tickTime;
      update(context.tickTime);
    }

    glClearColor(37.f / 255.f, 150.f / 255.f, 190.f / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw();
    //glErrorChecker(__LINE__);
    glfwPollEvents();

    if(fpsClock + 1.0 < frameStartTime) {
      fpsClock = frameStartTime;
      std::printf("FPS: %3d TICKS: %3d\n",  context.frameCounter, context.tickCounter);
      context.frameCounter = 0;
      context.tickCounter = 0;
      std::printf("Size: %llu\n", context.bombPositions.size());
    std::printf("anglechange : %f\n", context.playerChangeAngle);
    }
    context.frameCounter++;
  }

  context.texture.destruct();
  context.flower.destruct();
  glfwDestroyWindow(context.window);
  glfwTerminate();

  exit(EXIT_SUCCESS);
}

static void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  if(action == GLFW_PRESS) {
    context.pressedKeys[key] = true;
  }
  if(action == GLFW_RELEASE) {
    context.pressedKeys[key] = false;
  }

  if(key == GLFW_KEY_Q && action == GLFW_PRESS) {
    context.projectionSwitch ^= 1;
  }
  if(key == GLFW_KEY_Z && action == GLFW_PRESS) {
    context.reloadShader = true;
  }
}

static void windowResizeCallback(GLFWwindow* window, int width, int height) {
  context.width = width;
  context.height = height;
  glViewport(0, 0, width, height);

  context.aspectRatio = context.width / static_cast<float>(context.height);
}

static void updateCamMovement(double dt);

static void update(double dt) {
  updateCamMovement(dt);
  context.camera.update(dt);
}

static void updateCamMovement(double dt) {
  auto& pressedKeys = context.pressedKeys;
  auto& camera = context.camera;

  auto moveSpeed = 15.f;
  if(pressedKeys[GLFW_KEY_W]) {
    camera.camSpeed = moveSpeed;
  }
  if(pressedKeys[GLFW_KEY_S]) {
    camera.camSpeed = -moveSpeed;
  }
  if(pressedKeys[GLFW_KEY_A]) {
    camera.strafeSpeed = -moveSpeed;
  }
  if(pressedKeys[GLFW_KEY_D]) {
    camera.strafeSpeed = moveSpeed;
  }

  if(pressedKeys[GLFW_KEY_R]) {
    camera.pos().y += moveSpeed * dt;
  }
  if(pressedKeys[GLFW_KEY_F]) {
    camera.pos().y -= moveSpeed * dt;
  }

  auto rotSpeed = 30.f;
  if(pressedKeys[GLFW_KEY_LEFT]) {
    camera.rotateHor = rotSpeed;
  }
  if(pressedKeys[GLFW_KEY_RIGHT]) {
    camera.rotateHor = -rotSpeed;
  }
  if(pressedKeys[GLFW_KEY_UP]) {
    camera.rotateVert = -rotSpeed;
  }
  if(pressedKeys[GLFW_KEY_DOWN]) {
    camera.rotateVert = rotSpeed;
  }

  double x, y;
  glfwGetCursorPos(context.window, &x, &y);
  x /= float(context.width) * 0.5f;
  y /= float(context.height) * 0.5f;
  x = x - 1.0f;
  y = 1.0f - y;
  x *= context.aspectRatio;


  if(pressedKeys[GLFW_KEY_SPACE]) {
    context.bombPositions.emplace_back(context.playerPosition);
  }

  context.playerVelocity = glm::vec3(0.0f);
  auto maxVelocity = 20.f;
  if(pressedKeys[GLFW_KEY_W]) {
    context.playerVelocity.x = -maxVelocity;
  }
  if(pressedKeys[GLFW_KEY_S]) {
    context.playerVelocity.x = maxVelocity;
  }

  if(pressedKeys[GLFW_KEY_A]) {
    context.playerVelocity.z = maxVelocity;
  }
  if(pressedKeys[GLFW_KEY_D]) {
    context.playerVelocity.z = -maxVelocity;
  }
  auto tmp = glm::vec3(0.0f);
  if(context.playerVelocity != glm::vec3(0.0f)) {
    context.playerChangeAngle = std::atan2(-context.playerVelocity.z, context.playerVelocity.x) - context.playerDirectionAngle;
    if(std::fabs(context.playerChangeAngle) > glm::pi<float>()) {
      std::printf("%f\n", context.playerChangeAngle);
      if(context.playerChangeAngle < 0.0) {
        context.playerChangeAngle += glm::two_pi<float>();
      } else {
        context.playerChangeAngle -= glm::two_pi<float>();
      }
    }
    context.playerChangeAngle *= dt * 5;
    context.playerDirectionAngle += context.playerChangeAngle;
    tmp = glm::vec3(glm::cos(context.playerDirectionAngle), 0.0f, -glm::sin(context.playerDirectionAngle));
    tmp *= dt * maxVelocity;
  }

  context.playerVelocity *= dt;
  context.playerPosition += tmp;

  auto& grid = context.grid;
  for(size_t x = 0 ; x < grid.size(); x++) {
    for(size_t z = 0 ; z < grid[x].size(); z++) {
      if(grid[x][z] == 0) {
        continue;
      }
      auto boxPosition = glm::vec3(x * 2.0f + 1.0f, 0.0f, z * 2.0f + 1.0f);
      //auto 
      //context.playerPosition;
    }
  }
}

static void draw() {
  glm::mat4 v, p, mvp;

  if(context.projectionSwitch) {
    p = glm::ortho(-context.aspectRatio, context.aspectRatio, -1.f, 1.f, 1.f, -1.f);
    v = glm::mat4(1.0f);
    v = glm::translate(glm::vec3(0.0f, -0.8f, 0.0f));
  } else {
    p = glm::perspective(1.0f, context.aspectRatio, 0.5f, 300.0f);
    v = context.camera.view();

    constexpr auto camRadius = 18.f;
    constexpr auto camAngle = 0.7f * glm::half_pi<float>();
    constexpr auto camHeight = glm::vec3(
      std::cos(camAngle) * camRadius,
      std::sin(camAngle) * camRadius,
      0.0f
    );
    v = glm::lookAt(
      context.playerPosition + camHeight,
      context.playerPosition,
      glm::vec3(0.0f, 1.0f, 0.0f)
    );
  }

  context.s.use();
  glBindTexture(GL_TEXTURE_2D, context.texture.texture);
  glUniform1f(context.s.findUniformLocation("time"), glfwGetTime());

  for(size_t i = 0 ; i < context.bombPositions.size(); i++) {

    glm::mat4 m {1.0f};

    auto bombPosition = context.bombPositions[i];
    m *= glm::translate(bombPosition);
    m *= glm::rotate(float(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
    mvp = p * v * m;

    glUniformMatrix4fv(context.s.findUniformLocation("Model"), 1, GL_FALSE, glm::value_ptr(m));
    glUniformMatrix4fv(context.s.findUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

    context.bombMesh.render(context.s);
  }

  for(size_t x = 0 ; x < context.grid.size(); x++) {
    for(size_t z = 0 ; z < context.grid[x].size(); z++) {
      if(context.grid[x][z] == 0) {
        continue;
      }
      glm::mat4 m {1.0f};

      auto cubePosition = glm::vec3(2.0f * x, 0.0f, 2.0f * z);
      m *= glm::translate(cubePosition);
      mvp = p * v * m;

      glUniformMatrix4fv(context.s.findUniformLocation("Model"), 1, GL_FALSE, glm::value_ptr(m));
      glUniformMatrix4fv(context.s.findUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

      context.cube.render(context.s);
    }
  }

  glm::mat4 m {1.0f};

  auto levitatingVector = glm::vec3(0.0f, 0.3f, 0.0f);
  levitatingVector *= glm::sin(3*glfwGetTime());
  levitatingVector += glm::vec3(0.0f, 0.2f, 0.0f);
  m *= glm::translate(levitatingVector);

  m *= glm::translate(context.playerPosition);
  m *= glm::rotate(context.playerDirectionAngle, glm::vec3(0.0f, 1.0f, 0.0f));

  mvp = p * v * m;

  glUniformMatrix4fv(context.s.findUniformLocation("Model"), 1, GL_FALSE, glm::value_ptr(m));
  glUniformMatrix4fv(context.s.findUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

  context.playerMesh.render(context.s);

  glBindTexture(GL_TEXTURE_2D, context.flower.texture);
  m = glm::mat4(1.0f);
  m *= glm::rotate(glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
  mvp = p * v * m;
  glUniformMatrix4fv(context.s.findUniformLocation("Model"), 1, GL_FALSE, glm::value_ptr(m));
  glUniformMatrix4fv(context.s.findUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

  context.floor.render(context.s);

  glfwSwapBuffers(context.window);
}

static void init() {
  glfwSetErrorCallback(errorCallback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

  int monitorCount;
  GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
  for(int i = 0; i < monitorCount; i++) {
    GLFWmonitor* monitor = monitors[i];
    printf("Monitor%i: %s\n", i, glfwGetMonitorName(monitor));
  }

  auto constexpr SCALING = 32;
  context.window = glfwCreateWindow(16 * SCALING, 9 * SCALING, "GLFW1", NULL, NULL);
  if (!context.window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMaximizeWindow(context.window);

  glfwSetInputMode(context.window, GLFW_STICKY_KEYS, GLFW_TRUE);
  glfwSetKeyCallback(context.window, keyCallback);
  glfwSetWindowSizeCallback(context.window, windowResizeCallback);

  glfwMakeContextCurrent(context.window);
  glfwSwapInterval(1);

  glewExperimental = true;

  GLenum err = glewInit();
  if(GLEW_OK != err) {
    puts("glew does not workl");
    fprintf(stderr, "Error: %s\n", glewGetErrorString(GLEW_VERSION));
    exit(EXIT_FAILURE);
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glfwGetFramebufferSize(context.window, &context.width, &context.height);
  windowResizeCallback(context.window, context.width, context.height);
}