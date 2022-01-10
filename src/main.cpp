#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <stdlib.h>
#include <stdio.h>

#include "shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

double scale = 1.0;
bool projectionSwitch = false;
bool reloadShader = false;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    static constexpr auto scaleCoefficient = 1.02;
    if(key == GLFW_KEY_W && action == GLFW_PRESS) {
        scale *= scaleCoefficient;
    }
    if(key == GLFW_KEY_S) {
        scale /= scaleCoefficient;
    }
    if(key == GLFW_KEY_Q && action == GLFW_PRESS) {
      projectionSwitch ^= 1;
    }
    if(key == GLFW_KEY_Z && action == GLFW_PRESS) {
      reloadShader = true;
    }
}

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
  GLFWwindow* window;
  GLuint vertex_buffer;
  GLint mvp_location, vpos_location, vcol_location;

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
  window = glfwCreateWindow(16 * SCALING, 9 * SCALING, "Simple example", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  //glfwMaximizeWindow(window);

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glewExperimental = true;

  GLenum err = glewInit();
  if(GLEW_OK != err) {
    puts("glew does not workl");
    fprintf(stderr, "Error: %s\n", glewGetErrorString(GLEW_VERSION));
  }

  GLint result, error_log_size;

  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  Shader s("shader");
  //s.compile("shader");
  mvp_location = s.findUniformLocation("MVP");
  vpos_location = s.findVarLocation("vPos");
  vcol_location = s.findVarLocation("vCol");

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(
    vpos_location,
    2,
    GL_FLOAT,
    GL_FALSE,
    sizeof(vertices[0]),
    nullptr
  );
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(
    vcol_location,
    3,
    GL_FLOAT,
    GL_FALSE,
    sizeof(vertices[0]),
    (void*) (sizeof(float) * 2)
  );

  int width, height, channels;
  auto img_data = stbi_load("flower-texture.jpg", &width, &height, &channels, 0);
  if(!img_data) {
    perror("couldn't load data");
  }

  while (!glfwWindowShouldClose(window))
  {
    double startTime = glfwGetTime();

    if(reloadShader) {
      glDeleteProgram(s());
      s = Shader("shader");
    }
    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    x /= float(width) * 0.5f;
    y /= float(height) * 0.5f;
    x = x - 1.0f;
    y = 1.0f - y;
    x *= ratio;

    glm::mat4 m = glm::mat4(1.0f), v, p, mvp;
    m = glm::translate(m, glm::vec3(x, y, 0.0f));
    m *= glm::rotate(float(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
    //m = glm::scale(m, {scale, scale, scale});

    v = glm::lookAt({0.0f, 0.0f, 10.0f}, glm::vec3(), {0.0f, 1.0f, 0.0f});

    if(projectionSwitch) {
      p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      v = glm::lookAt({0.0f, 0.0f, 2.0f}, glm::vec3(), {0.0f, 1.0f, 0.0f});
      mvp = p * v * m;
    } else {
      mvp = p * v * m;
      p = glm::perspective(1.0f, ratio, 0.5f, 300.0f);
    }


    s.use();
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
    glDrawArrays(GL_TRIANGLES, 0, 3);
    //glErrorChecker(__LINE__);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  stbi_image_free(img_data);
  exit(EXIT_SUCCESS);
}
