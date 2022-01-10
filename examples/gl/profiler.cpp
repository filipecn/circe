/// Copyright (c) 2022, FilipeCN.
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
///\file profiler.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-01-05
///
///\brief

#include <circe/circe.h>
#include <hermes/common/profiler.h>
#include <thread>

struct ProfilerExample : public circe::gl::BaseApp {
  ProfilerExample() : circe::gl::BaseApp(800, 800) {
    hermes::profiler::Profiler::setMaxBlockCount(200);
  }

  void render(circe::CameraInterface *camera) override {
    {
      HERMES_PROFILE_FUNCTION(hermes::argb_colors::GreenA200)
      ImGui::SetNextWindowSize(ImVec2(400.0f, 400.0f), ImGuiCond_FirstUseEver);
      ImGui::Begin("GPU performance", nullptr);
      HERMES_PROFILE_START_BLOCK("stuff", hermes::argb_colors::Coral)
      for (int i = 0; i < 10; ++i) {
        HERMES_PROFILE_SCOPE("for", hermes::argb_colors::Blue300)
        std::this_thread::sleep_for(std::chrono::microseconds(500));
      }
      HERMES_PROFILE_END_BLOCK
      HERMES_PROFILE_SCOPE("GUI", hermes::argb_colors::BlueA200)
      profiler_view.render();
      logger_view.render();
      HERMES_LOG_VARIABLE(this->frame_counter_);
      if(this->frame_counter_ % 10 == 0)
        HERMES_LOG_WARNING("10th frame!")
      ImGui::End();
    }
    // for now we can only update when stack is empty
    profiler_view.update();
  }

  circe::HProfiler profiler_view;
  circe::HLogger logger_view;
};

int main() {
  return ProfilerExample().run();
}
