#include <circe/gl/io/graphics_display.h>
#include <circe/gl/io/viewport_display.h>

namespace circe::gl {

ViewportDisplay::ViewportDisplay(int x, int y, int width, int height, circe::viewport_options options) {
  resolution_.width = width;
  resolution_.height = height;
  position_.i = x;
  position_.j = y;
  renderer.reset(new DisplayRenderer(width, height));
  if (CIRCE_MASK_BIT(options, circe::viewport_options::orthographic))
    setCamera(UICamera::fromOrthographic());
  else
    setCamera(UICamera::fromPerspective());
  input_enabled_ = !CIRCE_MASK_BIT(options, circe::viewport_options::no_input);
}

void ViewportDisplay::render(const std::function<void(CameraInterface *)> &f) {
  if (prepareRenderCallback)
    prepareRenderCallback(*this);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  // TODO fix process
//  renderer->process([&]() {
//    if (f)
//      f(camera.get());
//    if (renderCallback)
//      renderCallback(camera.get());
//  });
  //  glDisable(GL_DEPTH_TEST);
  GraphicsDisplay &gd = GraphicsDisplay::instance();
  glViewport(position_.i, position_.j, resolution_.width, resolution_.height);
  glScissor(position_.i, position_.j, resolution_.width, resolution_.height);
  glEnable(GL_SCISSOR_TEST);
  circe::gl::GraphicsDisplay::clearScreen(clear_screen_color);
//  glEnable(GL_DEPTH_TEST);
// TODO fix post-process
//  renderer->render();
  if (renderCallback)
//    renderCallback(camera.get());
    renderCallback(&camera_);
  else if (f)
//    f(camera.get());
    f(&camera_);
  glDisable(GL_SCISSOR_TEST);
  if (renderEndCallback)
    renderEndCallback();
}

void ViewportDisplay::mouse(double x, double y) {
  if (!input_enabled_)
    return;
  if (mouseCallback)
    mouseCallback(x, y);
  camera_.mouseMove(getMouseNPos());
}

void ViewportDisplay::scroll(double dx, double dy) {
  if (!input_enabled_)
    return;
  camera_.mouseScroll(getMouseNPos(), hermes::vec2(dx, dy));
}

void ViewportDisplay::button(int b, int a, int m) {
  if (!input_enabled_)
    return;
  if (buttonCallback)
    buttonCallback(b, a, m);
  camera_.mouseButton(a, b, getMouseNPos());
}

void ViewportDisplay::key(int k, int scancode, int action, int modifiers) const {
  if (!input_enabled_)
    return;
  if (keyCallback)
    keyCallback(k, scancode, action, modifiers);
}

hermes::point2 ViewportDisplay::getMouseNPos() const {
  int vp[] = {0, 0, (int) resolution_.width, (int) resolution_.height};
  hermes::point2 mp =
      GraphicsDisplay::instance().getMousePos() - hermes::vec2(position_);
  return hermes::point2((mp.x - vp[0]) / vp[2] * 2.0 - 1.0,
                        (mp.y - vp[1]) / vp[3] * 2.0 - 1.0);
}

hermes::index2 ViewportDisplay::getMousePos() const {
  hermes::point2 mp =
      GraphicsDisplay::instance().getMousePos() - hermes::vec2(position_);
  return hermes::index2(mp.x, mp.y);
}

bool ViewportDisplay::hasMouseFocus() const {
  hermes::point2 mp =
      GraphicsDisplay::instance().getMousePos() - hermes::vec2(position_);
  return (mp.x >= 0.f && mp.x <= resolution_.width && mp.y >= 0.f && mp.y <= resolution_.height);
}

hermes::point3 ViewportDisplay::viewCoordToNormDevCoord(hermes::point3 p) const {
  float v[] = {0, 0, static_cast<float>(resolution_.width), static_cast<float>(resolution_.height)};
  return hermes::point3((p.x - v[0]) / (v[2] / 2.0) - 1.0,
                        (p.y - v[1]) / (v[3] / 2.0) - 1.0, 2 * p.z - 1.0);
}

hermes::point3 ViewportDisplay::unProject(const CameraInterface &c,
                                          hermes::point3 p) {
  return hermes::inverse(c.getTransform()) * p;
}

hermes::point3 ViewportDisplay::unProject(const hermes::point3 &p) const {
  return hermes::inverse(camera_.getTransform()) * p;
}

hermes::point3 ViewportDisplay::unProject() const {
  return hermes::inverse(camera_.getTransform()) *
      hermes::point3(getMouseNPos().x, getMouseNPos().y, 0.f);
}

hermes::index2 ViewportDisplay::project(const hermes::point3 &p) const {
  auto s = camera_.getTransform()(p);
  return {static_cast<int>((s.x + 1) * resolution_.width / 2), static_cast<int>((s.y + 1) * resolution_.height / 2)};
}

void ViewportDisplay::setCamera(const UICamera &viewport_camera) {
  camera_ = viewport_camera;
  camera_.resize(resolution_.width, resolution_.height);
}

void ViewportDisplay::resize(int w, int h) {
  camera_.resize(w, h);
  resolution_.width = w;
  resolution_.height = h;
}

const hermes::size2 &ViewportDisplay::size() const {
  return resolution_;
}

const hermes::index2 &ViewportDisplay::position() const {
  return position_;
}

void ViewportDisplay::set(const hermes::index2 &p, const hermes::size2 &s) {
  position_ = p;
  resize(s.width, s.height);
}

UICamera &ViewportDisplay::camera() {
  return camera_;
}

const UICamera &ViewportDisplay::camera() const {
  return camera_;
}

void ViewportDisplay::disableInput() {
  input_enabled_ = false;
}

void ViewportDisplay::enableInput() {
  input_enabled_ = true;
}

} // namespace circe
