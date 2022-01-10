#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>


#include <stdlib.h>
#include <stdio.h>


#include "shader.hpp"
#include "mesh.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static const struct
{
    float x, y;
    float r, g, b;
    float u, v;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f, 0.0f, 0.0f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f, 0.5f, 1.0f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f, 1.0f, 0.0f }
};

static struct Context {
  GLFWwindow* window;
  int width, height;
  float aspectRatio;
  Shader s;

  GLuint texture1;

  double scale = 1.0;
  bool projectionSwitch = false;
  bool reloadShader = false;

} context;

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void init();
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

  GLuint vertex_buffer;

  context.s = {"basic"};

  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glEnableVertexAttribArray(context.s.findVarLocation("vPos"));
  glVertexAttribPointer(
    context.s.findVarLocation("vPos"),
    2,
    GL_FLOAT,
    GL_FALSE,
    sizeof(vertices[0]),
    nullptr
  );
  glEnableVertexAttribArray(context.s.findVarLocation("vCol"));
  glVertexAttribPointer(
    context.s.findVarLocation("vCol"),
    3,
    GL_FLOAT,
    GL_FALSE,
    sizeof(vertices[0]),
    (void*) (sizeof(float) * 2)
  );
  glEnableVertexAttribArray(context.s.findVarLocation("vUV"));
  glVertexAttribPointer(
    context.s.findVarLocation("vUV"),
    2,
    GL_FLOAT,
    GL_FALSE,
    sizeof(vertices[0]),
    (void*) (sizeof(float) * 5)
  );

  glGenTextures(1, &context.texture1);
  glBindTexture(GL_TEXTURE_2D, context.texture1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  int width, height, channels;
  auto img_data = stbi_load("assets/flower-texture.jpg", &width, &height, &channels, 0);
  if(!img_data) {
    std::perror("couldn't load data");
  } else {
    std::printf("IMG_WIDTH: %d\n", width);
    std::printf("IMG_HEIGHT: %d\n", height);
    std::printf("IMG_CHANNELS: %d\n", channels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  stbi_image_free(img_data);

  Assimp::Importer importer;


  while (!glfwWindowShouldClose(context.window)) {
    double startTime = glfwGetTime();

    if(context.reloadShader) {
      context.reloadShader = false;
      glDeleteProgram(context.s());
      context.s = {"basic"};
    }

    glfwGetFramebufferSize(context.window, &context.width, &context.height);
    context.aspectRatio = context.width / static_cast<float>(context.height);

    glViewport(0, 0, context.width, context.height);
    glClearColor(37.f / 255.f, 150.f / 255.f, 190.f / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    draw();
    glErrorChecker(__LINE__);
    glfwPollEvents();
  }

  glfwDestroyWindow(context.window);

  glfwTerminate();
  glDeleteTextures(1, &context.texture1);
  exit(EXIT_SUCCESS);
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  static constexpr auto scaleCoefficient = 1.02;
  if(key == GLFW_KEY_W && action == GLFW_PRESS) {
    context.scale *= scaleCoefficient;
  }
  if(key == GLFW_KEY_S) {
    context.scale /= scaleCoefficient;
  }
  if(key == GLFW_KEY_Q && action == GLFW_PRESS) {
    context.projectionSwitch ^= 1;
  }
  if(key == GLFW_KEY_Z && action == GLFW_PRESS) {
    context.reloadShader = true;
  }
}

static void draw() {
  double x, y;
  glfwGetCursorPos(context.window, &x, &y);
  x /= float(context.width) * 0.5f;
  y /= float(context.height) * 0.5f;
  x = x - 1.0f;
  y = 1.0f - y;
  x *= context.aspectRatio;

  glm::mat4 m = glm::mat4(1.0f), v, p, mvp;
  m = glm::translate(m, glm::vec3(x, y, 0.0f));
  m *= glm::rotate(float(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));


  if(context.projectionSwitch) {
    p = glm::ortho(-context.aspectRatio, context.aspectRatio, -1.f, 1.f, 1.f, -1.f);
    //v = glm::lookAt({0.0f, 0.0f, 2.0f}, glm::vec3(), {0.0f, 1.0f, 0.0f});
    v = glm::mat4(1.0f);
  } else {
    p = glm::perspective(1.0f, context.aspectRatio, 0.5f, 300.0f);
    v = glm::lookAt({0.0f, 0.0f, 10.0f}, glm::vec3(), {0.0f, 1.0f, 0.0f});
  }
  mvp = p * v * m;

  context.s.use();
  glBindTexture(GL_TEXTURE_2D, context.texture1);
  glUniformMatrix4fv(context.s.findUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glfwSwapBuffers(context.window);
}

static void init() {
  glfwSetErrorCallback(error_callback);

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
  glfwSetKeyCallback(context.window, key_callback);

  glfwMakeContextCurrent(context.window);
  glfwSwapInterval(1);

  glewExperimental = true;

  GLenum err = glewInit();
  if(GLEW_OK != err) {
    puts("glew does not workl");
    fprintf(stderr, "Error: %s\n", glewGetErrorString(GLEW_VERSION));
    exit(EXIT_FAILURE);
  }
}