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

  Mesh mesh;
  Texture texture;

  double scale = 1.0;
  bool projectionSwitch = false;
  bool reloadShader = false;

  int frameCounter = 0;
  int tickCounter = 0;
  const double tickTime = 1.0 / 60.0;

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

  context.mesh.load("assets/obj/robo-boodie.obj");

  context.texture.gen("assets/textures/low-poly.png");

  context.camera.pos() = {0.0f, 0.0f, 5.0f};

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
    }
    context.frameCounter++;
  }

  context.texture.destruct();
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
    camera.strafeSpeed = moveSpeed;
  }
  if(pressedKeys[GLFW_KEY_D]) {
    camera.strafeSpeed = -moveSpeed;
  }

  if(pressedKeys[GLFW_KEY_R]) {
    camera.pos().y += moveSpeed * dt;
  }
  if(pressedKeys[GLFW_KEY_F]) {
    camera.pos().y -= moveSpeed * dt;
  }

  auto rotSpeed = 30.f;
  if(pressedKeys[GLFW_KEY_LEFT]) {
    camera.rotateHor = -rotSpeed;
  }
  if(pressedKeys[GLFW_KEY_RIGHT]) {
    camera.rotateHor = rotSpeed;
  }
  if(pressedKeys[GLFW_KEY_UP]) {
    camera.rotateVert = -rotSpeed;
  }
  if(pressedKeys[GLFW_KEY_DOWN]) {
    camera.rotateVert = rotSpeed;
  }
}

static void draw() {

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  double x, y;
  glfwGetCursorPos(context.window, &x, &y);
  x /= float(context.width) * 0.5f;
  y /= float(context.height) * 0.5f;
  x = x - 1.0f;
  y = 1.0f - y;
  x *= context.aspectRatio;

  glm::mat4 m = glm::mat4(1.0f), v, p, mvp;
  m *= glm::rotate(float(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
  //m = glm::translate(m, glm::vec3(x, y, 3.0f));


  if(context.projectionSwitch) {
    p = glm::ortho(-context.aspectRatio, context.aspectRatio, -1.f, 1.f, 1.f, -1.f);
    //v = glm::lookAt({0.0f, 0.0f, 2.0f}, glm::vec3(), {0.0f, 1.0f, 0.0f});
    v = glm::mat4(1.0f);
  } else {
    p = glm::perspective(1.0f, context.aspectRatio, 0.5f, 300.0f);
    v = context.camera.view();
  }
  mvp = p * v * m;

  glUniformMatrix4fv(context.s.findUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
  glBindTexture(GL_TEXTURE_2D, context.texture.texture);
  context.mesh.render(context.s);

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
  //glfwMaximizeWindow(window);

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

  windowResizeCallback(context.window, context.width, context.height);
}