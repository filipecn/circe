#include "circe.h"

namespace circe {

bool initialize() {
  static bool initialized = false;
  if (initialized)
    return true;
  if (!gladLoadGL()) {
    std::cerr << "GLAD failed.";
  }
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw std::runtime_error("Could not initialize GLAD!");
  glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
  // printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
  /*int gl_major, gl_minor;
  // Initialize the "OpenGL Extension Wrangler" library
  if (!initGLEW()) {
    std::cout << "glew init failed!\n";
    return false;
  }
  // Make sure that OpenGL 2.0 is supported by the driver
  getGlVersion(&gl_major, &gl_minor);
  printf("GL_VERSION major=%d minor=%d\n", gl_major, gl_minor);
  if (gl_major < 2) {
    printf("GL_VERSION major=%d minor=%d\n", gl_major, gl_minor);
    printf("Support for OpenGL 2.0 is required for this demo...exiting\n");
    //exit(1);
  }*/
  initialized = true;
  return true;
}

} // namespace circe
