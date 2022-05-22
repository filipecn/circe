#include <circe/gl/io/graphics_display.h>
#include <circe/gl/ui/app.h>

namespace circe::gl {

App::App(uint w, uint h, const char *t, circe::viewport_options viewport_config, circe::app_options options)
    : initialized_(false), window_width_(w), window_height_(h), title_(t) {
  prepareRenderCallback = nullptr;
  renderCallback = nullptr;
  finishRenderCallback = nullptr;
  mouseCallback = nullptr;
  buttonCallback = nullptr;
  keyCallback = nullptr;
  resizeCallback = nullptr;

  init();

  auto no_viewport = CIRCE_MASK_BIT(options, circe::app_options::no_viewport);

  if (no_viewport)
    return;

  addViewport(0, 0, window_width_, window_height_, viewport_config);
}

App::App(uint w, uint h, const char *t, circe::app_options options, circe::viewport_options viewport_config) :
    App(w, h, t, viewport_config, options) {}

size_t App::addViewport(uint x, uint y, uint w, uint h, circe::viewport_options options,
                        const std::function<void(circe::CameraInterface *)> &viewport_render_callback) {
  viewports_.emplace_back(x, y, w, h, options);
  viewports_.back().renderCallback = viewport_render_callback;
  return viewports_.size() - 1;
}

size_t App::addViewport3D(uint x, uint y, uint w, uint h,
                          const std::function<void(circe::CameraInterface *)> &viewport_render_callback) {
  viewports_.emplace_back(x, y, w, h);
  viewports_.back().setCamera(UICamera::fromPerspective());
  viewports_.back().renderCallback = viewport_render_callback;
  return viewports_.size() - 1;
}

size_t App::addViewport2D(uint x, uint y, uint w, uint h,
                          const std::function<void(circe::CameraInterface *)> &viewport_render_callback) {
  viewports_.emplace_back(x, y, w, h);
  viewports_.back().setCamera(UICamera::fromOrthographic());
  viewports_.back().renderCallback = viewport_render_callback;
  return viewports_.size() - 1;
}

void App::init() {
  if (initialized_)
    return;
  GraphicsDisplay &gd =
      createGraphicsDisplay(window_width_, window_height_, title_.c_str());
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
  initialized_ = true;
}

int App::run() {
  if (!initialized_)
    init();
  return GraphicsDisplay::instance().start();
}

void App::exit() { GraphicsDisplay::instance().stop(); }

void App::render() {
  if (prepareRenderCallback)
    prepareRenderCallback();
  for (current_rendering_viewport_ = 0; current_rendering_viewport_ < viewports_.size();
       ++current_rendering_viewport_)
    viewports_[current_rendering_viewport_].render(renderCallback);
  if (finishRenderCallback)
    finishRenderCallback();
}

void App::button(int button, int action, int modifiers) {
  for (auto &viewport : viewports_) {
    if (action == GLFW_PRESS && !viewport.hasMouseFocus())
      continue;
    viewport.button(button, action, modifiers);
  }
  if (buttonCallback)
    buttonCallback(button, action, modifiers);
}

void App::mouse(double x, double y) {
  for (auto &viewport : viewports_)
    viewport.mouse(x, y);
  if (mouseCallback)
    mouseCallback(x, y);
}

void App::scroll(double dx, double dy) {
  for (auto &viewport : viewports_)
    if (viewport.hasMouseFocus())
      viewport.scroll(dx, dy);
  if (scrollCallback)
    scrollCallback(dx, dy);
}

void App::key(int k, int scancode, int action, int modifiers) {
  for (auto &viewport : viewports_)
    viewport.key(k, scancode, action, modifiers);
  if (keyCallback)
    keyCallback(k, scancode, action, modifiers);
  else if (k == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    GraphicsDisplay::instance().stop();
}

void App::resize(int w, int h) {
  float wRatio = static_cast<float>(w) / window_width_;
  float hRatio = static_cast<float>(h) / window_height_;
  window_width_ = static_cast<uint>(w);
  window_height_ = static_cast<uint>(h);
  for (auto &viewport : viewports_) {
    auto vp = viewport.position();
    auto vs = viewport.size();
    viewport.set(hermes::index2(vp.i * wRatio, vp.j * hRatio),
                 hermes::size2(vs.width * wRatio, vs.height * hRatio));
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

size_t App::currentRenderingViewport() const {
  return current_rendering_viewport_;
}

const ViewportDisplay &App::viewport(size_t viewport_id) const {
  return viewports_[viewport_id];
}

ViewportDisplay &App::viewport(size_t viewport_id) {
  return viewports_[viewport_id];
}

} // namespace circe
