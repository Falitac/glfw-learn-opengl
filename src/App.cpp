#include "App.hpp"  

App* app;

App::App() {
  app = this;
}

void glErrorCallback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  if(action == GLFW_PRESS) {
    app->pressedKeys[key] = true;
  }
  if(action == GLFW_RELEASE) {
    app->pressedKeys[key] = false;
  }

  if(key == GLFW_KEY_Q && action == GLFW_PRESS) {
    app->projectionSwitch ^= 1;
  }
  if(key == GLFW_KEY_Z && action == GLFW_PRESS) {
    app->reloadShader = true;
  }
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
  app->width = width;
  app->height = height;
  app->aspectRatio = width / static_cast<float>(height);

  glViewport(0, 0, width, height);
}


void checkGLErrors(int line) {
  static int errorCount;
  GLenum glError;
  while((glError = glGetError()) != GL_NO_ERROR) {
    printf("Line: %d\n", line);
    printf("%0x\n", glError);
    errorCount++;
  }
}

void App::otherInit() {
  assetManager.addShader("basic", "basic");
  assetManager.addMesh("assets/obj/robo-boodie.obj", "robot");
  assetManager.addMesh("assets/obj/bomb.obj", "bomb");
  assetManager.addMesh("assets/obj/plane.obj", "plane");
  assetManager.addMesh("assets/obj/cube.obj", "cube");

  assetManager.addTexture("assets/textures/low-poly.png", "low-poly");
  assetManager.addTexture("assets/textures/flower-texture.jpg", "flower-texture");

  s = {"basic"};

  playerMesh.load("assets/obj/robo-boodie.obj");
  bombMesh.load("assets/obj/bomb.obj");
  floor.load("assets/obj/plane.obj");
  cube.load("assets/obj/cube.obj");

  texture.gen("assets/textures/low-poly.png");
  flower.gen("assets/textures/flower-texture.jpg");

  camera.pos() = {10.0f, 12.0f, 0.0f};
  camera.vertAngle() = -glm::quarter_pi<float>();
  camera.horAngle() = -glm::half_pi<float>();

  playerPosition = {5.0f, 0.0f, 5.0f};

  grid = std::vector(16, std::vector<int>(33, 0));

  for(int x = 0; x < 16; x++) {
    for(int z = 0; z < 33; z++) {
      if( x % 2 == 0 && z % 2 == 0) {
        grid[x][z] = 1;
      }
    }
  }

  for(auto it = grid.begin(); it != grid.end(); it++) {
    if(it == grid.begin() || it + 1 == grid.end()) {
      for(auto& element : *it) {
        element = 1;
      }
      continue;
    }
    it->front() = 1;
    it->back() = 1;
  }
}

/*
static void updateCamMovement() {
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
    camera.pos().y += moveSpeed * App::dt;
  }
  if(pressedKeys[GLFW_KEY_F]) {
    camera.pos().y -= moveSpeed * App::dt;
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
  glfwGetCursorPos(window, &x, &y);
  x /= float(width) * 0.5f;
  y /= float(height) * 0.5f;
  x = x - 1.0f;
  y = 1.0f - y;
  x *= aspectRatio;
}
*/

void App::run() {
  app = this;
  init();
  otherInit();
  loop();
}

void App::loop() {
  double timeAccumulator = 0.0;
  double lastFrameTime = 0.0;
  double fpsClock = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    double frameStartTime = glfwGetTime();
    double frameTimeDifference = frameStartTime - lastFrameTime;
    lastFrameTime = frameStartTime;
    timeAccumulator += frameTimeDifference;

    if(reloadShader) {
      reloadShader = false;
      glDeleteProgram(s());
      s = {"basic"};
    }

    while(timeAccumulator > dt) {
      tickCounter++;
      timeAccumulator -= dt;
      update();
    }

    glClearColor(0.f / 255.f, 0.f / 255.f, 139.f / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render();
    glfwSwapBuffers(window);
    //glErrorChecker(__LINE__);
    glfwPollEvents();

    if(fpsClock + 1.0 < frameStartTime) {
      fpsClock = frameStartTime;
      std::printf("FPS: %3d TICKS: %3d\n",  frameCounter, tickCounter);
      frameCounter = 0;
      tickCounter = 0;
      std::printf("%f %f\n", playerPosition.x, playerPosition.z);
    }
    frameCounter++;
  }
}

void App::update() {
  //updateCamMovement();

  if(pressedKeys[GLFW_KEY_SPACE]) {
    bombPositions.emplace_back(playerPosition);
  }

  playerVelocity = glm::vec3(0.0f);
  auto maxVelocity = 20.f;
  if(pressedKeys[GLFW_KEY_W]) {
    playerVelocity.x -= maxVelocity;
  }
  if(pressedKeys[GLFW_KEY_S]) {
    playerVelocity.x += maxVelocity;
  }

  if(pressedKeys[GLFW_KEY_A]) {
    playerVelocity.z += maxVelocity;
  }
  if(pressedKeys[GLFW_KEY_D]) {
    playerVelocity.z -= maxVelocity;
  }

  auto tmp = glm::vec3(0.0f);
  if(playerVelocity != glm::vec3(0.0f)) {
    playerChangeAngle = std::atan2(-playerVelocity.z, playerVelocity.x) - playerDirectionAngle;
    playerChangeAngle = std::fmod(playerChangeAngle, glm::two_pi<float>());

    if(playerChangeAngle > glm::pi<float>()) {
      playerChangeAngle -=  glm::two_pi<float>();
    }

    playerChangeAngle *= dt * 5;
    playerDirectionAngle += playerChangeAngle;
    tmp = glm::vec3(glm::cos(playerDirectionAngle), 0.0f, -glm::sin(playerDirectionAngle));
    tmp *= dt * maxVelocity;
  }

  playerVelocity *= dt;
  playerPosition += tmp;

  auto directionVector = [&](const auto& target) {
    std::array<glm::vec2, 4> compass = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++) {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return best_match;
  };

  auto playerIntersectBox = [&](const auto& boxCenter) {
    const auto boxSize = 2.0f;
    const auto playerRadius = 0.8f;
    glm::vec2 playerCenter = glm::vec2(playerPosition.x, playerPosition.z);
    glm::vec2 halfBox = glm::vec2(boxSize, boxSize);
    halfBox /= 2.0f;

    auto diff = playerCenter - boxCenter;
    auto clamped = glm::clamp(diff, -halfBox, halfBox);
    auto closest = boxCenter + clamped;
    diff = closest - playerCenter;

    auto result = glm::dot(diff, diff) <= playerRadius * playerRadius;

    if(result) {
      auto dir = directionVector(-playerCenter + boxCenter);
      if(dir == 0 || dir == 2) {
        auto offset = playerRadius - std::abs(diff.y);
        if(dir == 0) {
          playerPosition.z -= offset;
        } else {
          playerPosition.z += offset;
        }
      } else {
        auto offset = playerRadius - std::abs(diff.x);
        if(dir == 1) {
          playerPosition.x -= offset;
        } else {
          playerPosition.x += offset;
        }

      }
    }

    return result;
  };

  for(size_t x = 0 ; x < grid.size(); x++) {
    for(size_t z = 0 ; z < grid[x].size(); z++) {
      if(grid[x][z] == 0) {
        continue;
      }
      auto boxPosition = glm::vec2(x * 2.0f, z * 2.0f);
      if(playerIntersectBox(boxPosition)) {
        //grid[x][z] = 0;
      }
    }
  }
}

void App::render() {
  glm::mat4 v, p, mvp;

  constexpr auto camRadius = 25.f;
  constexpr auto camAngle = 0.7f * glm::half_pi<float>();
  constexpr auto camHeight = glm::vec3(
    std::cos(camAngle) * camRadius,
    std::sin(camAngle) * camRadius,
    0.0f
  );
  auto camPos = camHeight + playerPosition;
  if(projectionSwitch) {
    p = glm::ortho(-aspectRatio, aspectRatio, -1.f, 1.f, 1.f, -1.f);
    v = glm::mat4(1.0f);
    v = glm::translate(glm::vec3(0.0f, -0.8f, 0.0f));
  } else {
    p = glm::perspective(1.0f, aspectRatio, 0.5f, 300.0f);
    v = camera.view();

    v = glm::lookAt(
      playerPosition + camHeight,
      playerPosition,
      glm::vec3(0.0f, 1.0f, 0.0f)
    );
  }

  s.use();
  glBindTexture(GL_TEXTURE_2D, texture.texture);
  glUniform1f(s.findUniformLocation("time"), glfwGetTime());
  glUniform3fv(s.findUniformLocation("playerPos"), 1, glm::value_ptr(playerPosition));
  glUniform3fv(s.findUniformLocation("camPos"), 1, glm::value_ptr(camPos));

  for(size_t i = 0 ; i < bombPositions.size(); i++) {

    glm::mat4 m {1.0f};

    auto bombPosition = bombPositions[i];
    m *= glm::translate(bombPosition);
    m *= glm::rotate(float(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
    mvp = p * v * m;

    glUniformMatrix4fv(s.findUniformLocation("Model"), 1, GL_FALSE, glm::value_ptr(m));
    glUniformMatrix4fv(s.findUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

    bombMesh.render(s);
  }

  for(size_t x = 0 ; x < grid.size(); x++) {
    for(size_t z = 0 ; z < grid[x].size(); z++) {
      if(grid[x][z] == 0) {
        continue;
      }
      glm::mat4 m {1.0f};

      auto cubePosition = glm::vec3(2.0f * x, 0.0f, 2.0f * z);
      m *= glm::translate(cubePosition);
      mvp = p * v * m;

      glUniformMatrix4fv(s.findUniformLocation("Model"), 1, GL_FALSE, glm::value_ptr(m));
      glUniformMatrix4fv(s.findUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

      cube.render(s);
    }
  }

  glm::mat4 m {1.0f};

  auto levitatingVector = glm::vec3(0.0f, 0.3f, 0.0f);
  levitatingVector *= glm::sin(3*glfwGetTime());
  levitatingVector += glm::vec3(0.0f, 0.2f, 0.0f);
  m *= glm::translate(levitatingVector);

  m *= glm::translate(playerPosition);
  m *= glm::rotate(playerDirectionAngle, glm::vec3(0.0f, 1.0f, 0.0f));

  mvp = p * v * m;

  glUniformMatrix4fv(s.findUniformLocation("Model"), 1, GL_FALSE, glm::value_ptr(m));
  glUniformMatrix4fv(s.findUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

  playerMesh.render(s);

  glBindTexture(GL_TEXTURE_2D, flower.texture);
  m = glm::mat4(1.0f);
  m *= glm::rotate(glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
  mvp = p * v * m;
  glUniformMatrix4fv(s.findUniformLocation("Model"), 1, GL_FALSE, glm::value_ptr(m));
  glUniformMatrix4fv(s.findUniformLocation("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

  floor.render(s);

}

void App::init() {
  glfwSetErrorCallback(glErrorCallback);

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
  window = glfwCreateWindow(16 * SCALING, 9 * SCALING, "GLFW1", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMaximizeWindow(window);

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetWindowSizeCallback(window, windowResizeCallback);

  glfwMakeContextCurrent(window);
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

  glfwGetFramebufferSize(window, &width, &height);
  windowResizeCallback(window, width, height);
}

void App::cleanup() {
  texture.destruct();
  flower.destruct();
  glfwDestroyWindow(window);
  glfwTerminate();
}

