/// Copyright (c) 2020, FilipeCN.
///
/// The MIT License (MIT)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
///
///\file base_app.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-06-18
///
///\brief

#include "base_app.h"
#include <circe/imgui/imgui.h>
#include <circe/gl/imgui/imgui_impl_glfw.h>
#include <circe/gl/imgui/imgui_impl_opengl3.h>

namespace circe::gl {

using namespace circe::gl;

BaseApp::~BaseApp() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void BaseApp::prepare() {
  GraphicsDisplay::instance().mouseCallback = [&](double x, double y) {
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
      app_->mouse(x, y);
  };
  GraphicsDisplay::instance().buttonCallback = [&](int b, int a, int m) {
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
      app_->button(b, a, m);
  };
  GraphicsDisplay::instance().scrollCallback = [&](double x, double y) {
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
      app_->scroll(x, y);
  };
  // Setup Dear ImGui context
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void) io;
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(GraphicsDisplay::instance().getGLFWwindow(), true);
  const char *glsl_version = "#version 130";
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void BaseApp::prepareFrame(const ViewportDisplay &display) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void BaseApp::finishFrame() {
  // Rendering
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void BaseApp::nextFrame() {
  // start frame time
  auto t_start = std::chrono::high_resolution_clock::now();
  // update scene
  render(app_->getCamera());
  finishFrame();
  frame_counter_++;
  auto t_end = std::chrono::high_resolution_clock::now();
  auto t_diff = std::chrono::duration<double, std::milli>(t_end - t_start).count();
  frame_timer = (float) t_diff / 1000.0f;
  // Convert to clamped timer value
  float fps_timer = (float) (std::chrono::duration<double, std::milli>(t_end - last_timestamp_).count());
  if (fps_timer > 1000.0f) {
    last_FPS_ = static_cast<uint32_t>((float) frame_counter_ * (1000.0f / fps_timer));
    frame_counter_ = 0;
    last_timestamp_ = t_end;
  }
}

int BaseApp::run() {
  return app_->run();
}

} // circe::gl namespace