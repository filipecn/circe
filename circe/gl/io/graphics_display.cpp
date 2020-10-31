/*
 * Copyright (c) 2017 FilipeCN
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <circe/circe.h>

namespace circe::gl {

GraphicsDisplay GraphicsDisplay::instance_;

GraphicsDisplay::GraphicsDisplay()
    : window(nullptr), title(nullptr), width(400), height(400) {
  renderCallback = nullptr;
  buttonCallback = nullptr;
  keyCallback = nullptr;
  mouseCallback = nullptr;
  scrollCallback = nullptr;
  keyCallback = nullptr;
}

GraphicsDisplay::~GraphicsDisplay() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void GraphicsDisplay::set(int w, int h, const char *windowTitle) {
  width = w;
  height = h;
  title = windowTitle;
  window = nullptr;
  init();
}

bool GraphicsDisplay::init() {
  glfwSetErrorCallback(&glfwError);
  if (!glfwInit())
    return false;
  glfwSetTime(0);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_RED_BITS, 8);
  glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8);
  glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
  window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window) {
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);
  glfwSetCharCallback(window, char_callback);
  glfwSetDropCallback(window, drop_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, button_callback);
  glfwSetCursorPosCallback(window, pos_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetWindowSizeCallback(window, resize_callback);
  initialize();
  return true;
}

int GraphicsDisplay::start() {
  while (!glfwWindowShouldClose(this->window)) {
    glfwGetFramebufferSize(window, &this->width, &this->height);
    glViewport(0, 0, this->width, this->height);
    if (this->renderCallback) {
      this->renderCallback();
    }
    for (const auto &c : renderCallbacks)
      c();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}

bool GraphicsDisplay::isRunning() {
  return !glfwWindowShouldClose(this->window);
}

void GraphicsDisplay::getWindowSize(int &w, int &h) {
  w = this->width;
  h = this->height;
}

ponos::point2 GraphicsDisplay::getMousePos() {
  double x, y;
  glfwGetCursorPos(this->window, &x, &y);
  return ponos::point2(x, this->height - y);
}

ponos::point2 GraphicsDisplay::getMouseNPos() {
  int viewport[] = {0, 0, width, height};
  ponos::point2 mp = getMousePos();
  return ponos::point2((mp.x - viewport[0]) / viewport[2] * 2.0 - 1.0,
                       (mp.y - viewport[1]) / viewport[3] * 2.0 - 1.0);
}

ponos::point3 GraphicsDisplay::normDevCoordToViewCoord(ponos::point3 p) {
  ponos::point3 sp;
  sp.x = ponos::lerp(ponos::linearStep(p.x, -1.f, 1.f), 0.f,
                     static_cast<float>(width));
  sp.y = ponos::lerp(ponos::linearStep(p.y, -1.f, 1.f), 0.f,
                     static_cast<float>(height));
  return sp;
}

ponos::point3 GraphicsDisplay::viewCoordToNormDevCoord(ponos::point3 p) {
  float v[] = {0, 0, static_cast<float>(width), static_cast<float>(height)};
  return ponos::point3((p.x - v[0]) / (v[2] / 2.0) - 1.0,
                       (p.y - v[1]) / (v[3] / 2.0) - 1.0, 2 * p.z - 1.0);
}

ponos::point3 GraphicsDisplay::unProject(const CameraInterface &c,
                                         ponos::point3 p) {
  return ponos::inverse(c.getTransform()) * p;
}

void GraphicsDisplay::stop() { glfwSetWindowShouldClose(window, GL_TRUE); }

void GraphicsDisplay::beginFrame() {
  glfwGetFramebufferSize(window, &this->width, &this->height);
  glViewport(0, 0, this->width, this->height);
}

void GraphicsDisplay::endFrame() { glfwSwapBuffers(window); }

void GraphicsDisplay::clearScreen(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsDisplay::processInput() { glfwPollEvents(); }

int GraphicsDisplay::keyState(int key) { return glfwGetKey(window, key); }

void GraphicsDisplay::error_callback(int error, const char *description) {
  UNUSED_VARIABLE(error);
  fputs(description, stderr);
}

void GraphicsDisplay::registerRenderFunc(const std::function<void()> &f) {
  renderCallbacks.push_back(f);
}

/////////////////////////// CHAR FUNCTIONS
//////////////////////////////////////////////////////////
void GraphicsDisplay::registerCharFunc(
    const std::function<void(unsigned int)> &f) {
  charCallbacks.push_back(f);
}

void GraphicsDisplay::charFunc(unsigned int codepoint) {
  UNUSED_VARIABLE(codepoint);
}

void GraphicsDisplay::char_callback(GLFWwindow *window,
                                    unsigned int codepoint) {
  UNUSED_VARIABLE(window);
  if (instance_.keyCallback)
    instance_.charCallback(codepoint);
  else
    instance_.charFunc(codepoint);
  for (const auto &c : instance_.charCallbacks)
    c(codepoint);
}

/////////////////////////// DROP FUNCTIONS
//////////////////////////////////////////////////////////
void GraphicsDisplay::registerDropFunc(
    const std::function<void(int, const char **)> &f) {
  dropCallbacks.push_back(f);
}

void GraphicsDisplay::dropFunc(int count, const char **filenames) {
  UNUSED_VARIABLE(count);
  UNUSED_VARIABLE(filenames);
}

void GraphicsDisplay::drop_callback(GLFWwindow *window, int count,
                                    const char **filenames) {
  UNUSED_VARIABLE(window);
  if (instance_.keyCallback)
    instance_.dropCallback(count, filenames);
  else
    instance_.dropFunc(count, filenames);
  for (const auto &c : instance_.dropCallbacks)
    c(count, filenames);
}

/////////////////////////// KEY FUNCTIONS
//////////////////////////////////////////////////////////
void GraphicsDisplay::registerKeyFunc(
    const std::function<void(int, int, int, int)> &f) {
  keyCallbacks.push_back(f);
}

void GraphicsDisplay::key_callback(GLFWwindow *window, int key, int scancode,
                                   int action, int mods) {
  UNUSED_VARIABLE(window);
  if (instance_.keyCallback)
    instance_.keyCallback(key, scancode, action, mods);
  else
    instance_.keyFunc(key, scancode, action, mods);
  for (const auto &c : instance_.keyCallbacks)
    c(key, scancode, action, mods);
}

void GraphicsDisplay::keyFunc(int key, int scancode, int action,
                              int modifiers) {
  UNUSED_VARIABLE(scancode);
  UNUSED_VARIABLE(modifiers);
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// BUTTON FUNCTIONS
/////////////////////////////////////////////////////
void GraphicsDisplay::registerButtonFunc(
    const std::function<void(int, int, int)> &f) {
  buttonCallbacks.push_back(f);
}

void GraphicsDisplay::button_callback(GLFWwindow *window, int button,
                                      int action, int mods) {
  UNUSED_VARIABLE(window);
  if (instance_.buttonCallback)
    instance_.buttonCallback(button, action, mods);
  else
    instance_.buttonFunc(button, action, mods);
  for (const auto &c : instance_.buttonCallbacks)
    c(button, action, mods);
}

void GraphicsDisplay::buttonFunc(int button, int action, int modifiers) {
  UNUSED_VARIABLE(button);
  UNUSED_VARIABLE(action);
  UNUSED_VARIABLE(modifiers);
}

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// MOUSE MOTION FUNCTIONS
///////////////////////////////////////////////
void GraphicsDisplay::registerMouseFunc(
    const std::function<void(double, double)> &f) {
  mouseCallbacks.push_back(f);
}

void GraphicsDisplay::pos_callback(GLFWwindow *window, double x, double y) {
  UNUSED_VARIABLE(window);
  if (instance_.mouseCallback)
    instance_.mouseCallback(x, y);
  else
    instance_.mouseFunc(x, y);
  for (const auto &c : instance_.mouseCallbacks)
    c(x, y);
}

void GraphicsDisplay::mouseFunc(double x, double y) {
  UNUSED_VARIABLE(x);
  UNUSED_VARIABLE(y);
}

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// MOUSE SCROLL FUNCTIONS
///////////////////////////////////////////////
void GraphicsDisplay::registerScrollFunc(
    const std::function<void(double, double)> &f) {
  scrollCallbacks.push_back(f);
}

void GraphicsDisplay::scroll_callback(GLFWwindow *window, double x, double y) {
  UNUSED_VARIABLE(window);
  if (instance_.scrollCallback)
    instance_.scrollCallback(x, y);
  else
    instance_.scrollFunc(x, y);
  for (const auto &c : instance_.scrollCallbacks)
    c(x, y);
}

void GraphicsDisplay::scrollFunc(double x, double y) {
  UNUSED_VARIABLE(x);
  UNUSED_VARIABLE(y);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsDisplay::registerResizeFunc(
    const std::function<void(int, int)> &f) {
  resizeCallbacks.push_back(f);
}

void GraphicsDisplay::resize_callback(GLFWwindow *window, int w, int h) {
  UNUSED_VARIABLE(window);
  instance_.resizeFunc(w, h);
  if (instance_.resizeCallback) {
    instance_.getWindowSize(w, h);
    instance_.resizeCallback(w, h);
  }
  for (const auto &c : instance_.resizeCallbacks)
    c(w, h);
}

void GraphicsDisplay::resizeFunc(int w, int h) {
  UNUSED_VARIABLE(w);
  UNUSED_VARIABLE(h);
  glfwGetFramebufferSize(window, &this->width, &this->height);
}

GLFWwindow *GraphicsDisplay::getGLFWwindow() { return window; }

} // namespace circe
