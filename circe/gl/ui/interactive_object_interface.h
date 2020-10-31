#ifndef circe_UI_INTERACTIVE_OBJECT_INTERFACE_H
#define circe_UI_INTERACTIVE_OBJECT_INTERFACE_H

#include <circe/ui/trackball_interface.h>

namespace circe::gl {

/// An object that inherits from Interactive Object Interface carries a
/// trackball field and handles user input.
class InteractiveObjectInterface {
public:
  InteractiveObjectInterface() : selected(false), active(false) {}
  virtual ~InteractiveObjectInterface() {}

  virtual void mouse(CameraInterface &camera, ponos::point2 p) {
    trackball.mouseMove(camera, p);
    updateTransform();
  }

  virtual void button(CameraInterface &camera, ponos::point2 p, int button,
                      int action) {
    if (action == GLFW_RELEASE) {
      trackball.buttonRelease(camera, button, p);
      active = false;
    } else {
      trackball.buttonPress(camera, button, p);
      active = true;
    }
    updateTransform();
  }

  virtual void updateTransform() = 0;

  bool selected;
  bool active;
  TrackballInterface trackball;
};

} // namespace circe

#endif // circe_UI_INTERACTIVE_OBJECT_INTERFACE_H
