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

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
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

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);


    glewExperimental = true;

    GLenum err = glewInit();
    if(GLEW_OK != err) {
      puts("glew does not workl");
      fprintf(stderr, "Error: %s\n", glewGetErrorString(GLEW_VERSION));
    }
    Shader s;
    s.compile("shader");

    // NOTE: OpenGL error checks have been omitted for brevity
    GLint result, error_log_size;

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &error_log_size);

    if(error_log_size > 0) {
        char error_message[1000];
        std::string se;
        se.reserve(error_log_size);
        glGetShaderInfoLog(vertex_shader, 1000, NULL, &se[0]);
        fprintf(stderr, "Error compiling vertex shader %s\n", se.c_str());
        exit(1);
    }
    

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &error_log_size);

    if(error_log_size > 0) {
        char error_message[1000];
        glGetShaderInfoLog(vertex_shader, 1000, NULL, error_message);
        fprintf(stderr, "Error compiling fragment shader %s\n", error_message);
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &error_log_size);

    if(error_log_size > 0) {
        char error_message[1000];
        glGetProgramInfoLog(program, 1000, NULL, error_message);
        fprintf(stderr, "Error compiling program %s\n", error_message);
    }

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
    std::printf("Locations %d %d %d\n", mvp_location, vpos_location, vcol_location);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));

    while (!glfwWindowShouldClose(window))
    {
      float ratio;
      int width, height;
      glm::mat4 m, p, mvp;

      glfwGetFramebufferSize(window, &width, &height);
      ratio = width / (float) height;

      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);

      /*
      mat4x4_identity(m);
      mat4x4_rotate_Z(m, m, (float) glfwGetTime());
      mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      mat4x4_mul(mvp, p, m);
      */
        m = glm::rotate(float(glfwGetTime()), glm::vec3(0.0, 0.0, 1.0));

      p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);

      mvp = p * m;


      //glUseProgram(program);
      s.use();
      glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
      glDrawArrays(GL_TRIANGLES, 0, 3);
      glErrorChecker(__LINE__);

      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
