#ifndef CIRCE_IO_USER_INPUT_H
#define CIRCE_IO_USER_INPUT_H

#include <circe/gl/utils/open_gl.h>
#include <string>

namespace circe::gl {

class UserInput {
public:
#define MOUSE_BUTTON_LAST GLFW_MOUSE_BUTTON_LAST
#define MOUSE_BUTTON_LEFT GLFW_MOUSE_BUTTON_LEFT
#define MOUSE_BUTTON_RIGHT GLFW_MOUSE_BUTTON_RIGHT
#define MOUSE_BUTTON_MIDDLE GLFW_MOUSE_BUTTON_MIDDLE
#define MOUSE_SCROLL GLFW_DONT_CARE
#define PRESS GLFW_PRESS
#define RELEASE GLFW_RELEASE
  static std::string inputName(int i) {
    switch (i) {
    case MOUSE_BUTTON_LAST:
      return std::string("MOUSE_BUTTON_LAST");
    case MOUSE_BUTTON_LEFT:
      return std::string("MOUSE_BUTTON_LEFT");
    case MOUSE_BUTTON_RIGHT:
      return std::string("MOUSE_BUTTON_RIGHT");
    case MOUSE_BUTTON_MIDDLE:
      return std::string("MOUSE_BUTTON_MIDDLE");
    case MOUSE_SCROLL:
      return std::string("MOUSE_SCROLL");
    default:
      return std::string("INVALID_INPUT");
    }
    switch (i) {
    case PRESS:
      return std::string("PRESS");
    case RELEASE:
      return std::string("RELEASE");
    default:
      return std::string("INVALID_INPUT");
    }
  }
};

} // aergia namespace

#endif // CIRCE_IO_USER_INPUT_H
