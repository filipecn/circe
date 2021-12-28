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
///\file base_app.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-06-18
///
///\brief

#ifndef CIRCE_UTILS_BASE_APP_H
#define CIRCE_UTILS_BASE_APP_H

#include <circe/gl/ui/scene_app.h>
#include <chrono>

namespace circe::gl {

class BaseApp {
public:
  template<typename... Args>
  explicit BaseApp(Args &&... args) {
    app_ = std::make_unique<circe::gl::App>(std::forward<Args>(args)...);
    app_->finishRenderCallback = [&]() { this->endFrame(); };
    app_->prepareRenderCallback = [&]() { this->startFrame(); };
    app_->renderCallback = [&](circe::CameraInterface *c) { this->nextFrame(c); };
  }
  virtual ~BaseApp();
  void init();
  virtual void prepare();
  // render
  virtual void prepareFrame();
  virtual void render(circe::CameraInterface *camera) = 0;
  virtual void finishFrame();
  int run();

  ///  Last frame time measured using a high performance timer (if available)
  float frame_timer = 1.0f;

protected:
  void startFrame();
  void nextFrame(circe::CameraInterface *camera);
  void endFrame();

  std::unique_ptr<circe::gl::App> app_;
  // Frame counter to display fps
  std::chrono::time_point<std::chrono::high_resolution_clock> t_start;
  uint32_t frame_counter_ = 0;
  uint32_t last_FPS_ = 0;
  std::chrono::time_point<std::chrono::high_resolution_clock> last_timestamp_;
};

} // circe namespace

#endif //CIRCE_UTILS_BASE_APP_H