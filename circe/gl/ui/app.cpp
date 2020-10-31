#include <circe/gl/io/graphics_display.h>
#include <circe/gl/ui/app.h>

namespace circe::gl {

App::App(uint w, uint h, const char *t, bool defaultViewport)
    : initialized(false), windowWidth(w), windowHeight(h), title(t) {
  renderCallback = nullptr;
  mouseCallback = nullptr;
  buttonCallback = nullptr;
  keyCallback = nullptr;
  resizeCallback = nullptr;
  init();
  if (defaultViewport)
    addViewport(0, 0, windowWidth, windowHeight);
}

size_t App::addViewport(uint x, uint y, uint w, uint h) {
  viewports.emplace_back(x, y, w, h);
  viewports[viewports.size() - 1].camera.reset(new UserCamera3D());
  TrackballInterface::createDefault3D(
      viewports[viewports.size() - 1].camera->trackball);
  dynamic_cast<UserCamera3D *>(viewports[viewports.size() - 1].camera.get())
      ->resize(w, h);
  return viewports.size() - 1;
}

size_t App::addViewport2D(uint x, uint y, uint w, uint h) {
  viewports.emplace_back(x, y, w, h);
  viewports[viewports.size() - 1].camera.reset(new UserCamera2D());
  TrackballInterface::createDefault2D(
      viewports[viewports.size() - 1].camera->trackball);
  dynamic_cast<UserCamera2D *>(viewports[viewports.size() - 1].camera.get())
      ->resize(w, h);
  return viewports.size() - 1;
}

void App::init() {
  if (initialized)
    return;
  GraphicsDisplay &gd =
      createGraphicsDisplay(windowWidth, windowHeight, title.c_str());
  gd.charCallback = [this](unsigned char pointcode) { charFunc(pointcode); };
  gd.dropCallback = [this](int count, const char **filenames) {
    drop(count, filenames);
  };
  gd.renderCallback = [this]() { render(); };
  gd.buttonCallback = [this](int b, int a, int m) { button(b, a, m); };
  gd.mouseCallback = [this](double x, double y) { mouse(x, y); };
  gd.scrollCallback = [this](double dx, double dy) { scroll(dx, dy); };
  gd.keyCallback = [this](int k, int s, int a, int m) { key(k, s, a, m); };
  gd.resizeCallback = [this](int w, int h) { resize(w, h); };
  initialized = true;
}

int App::run() {
  if (!initialized)
    init();
  return GraphicsDisplay::instance().start();
}

void App::exit() { GraphicsDisplay::instance().stop(); }

void App::render() {
  for (auto &viewport : viewports)
    viewport.render();
  if (renderCallback)
    renderCallback();
}

void App::button(int button, int action, int modifiers) {
  for (auto &viewport : viewports) {
    if (action == GLFW_PRESS && !viewport.hasMouseFocus())
      continue;
    viewport.button(button, action, modifiers);
  }
  if (buttonCallback)
    buttonCallback(button, action, modifiers);
}

void App::mouse(double x, double y) {
  for (auto &viewport : viewports)
    viewport.mouse(x, y);
  if (mouseCallback)
    mouseCallback(x, y);
}

void App::scroll(double dx, double dy) {
  for (auto &viewport : viewports)
    if (viewport.hasMouseFocus())
      viewport.scroll(dx, dy);
  if (scrollCallback)
    scrollCallback(dx, dy);
}

void App::key(int k, int scancode, int action, int modifiers) {
  for (auto &viewport : viewports)
    viewport.key(k, scancode, action, modifiers);
  if (keyCallback)
    keyCallback(k, scancode, action, modifiers);
  else if (k == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    GraphicsDisplay::instance().stop();
}

void App::resize(int w, int h) {
  float wRatio = static_cast<float>(w) / windowWidth;
  float hRatio = static_cast<float>(h) / windowHeight;
  windowWidth = static_cast<uint>(w);
  windowHeight = static_cast<uint>(h);
  for (auto &viewport : viewports) {
    viewport.x *= wRatio;
    viewport.y *= hRatio;
    viewport.width *= wRatio;
    viewport.height *= hRatio;
    if (viewport.camera)
      viewport.camera->resize(viewport.width, viewport.height);
  }
}

void App::charFunc(unsigned int pointcode) {
  if (charCallback)
    charCallback(pointcode);
}

void App::drop(int count, const char **filenames) {
  if (dropCallback)
    dropCallback(count, filenames);
}

} // namespace circe
