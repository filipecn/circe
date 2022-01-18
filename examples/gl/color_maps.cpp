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
///\file color_maps.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-01-17
///
///\brief

#include <circe/circe.h>

struct ColorMapsExample : public circe::gl::BaseApp {
  ColorMapsExample() : circe::gl::BaseApp(800, 800, "Color Maps Example") {
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    HERMES_ASSERT_WITH_LOG(cube.program.link(shaders_path, "color"), cube.program.err);
    cube = circe::Shapes::box(hermes::bbox3::unitBox());
  }

  void render(circe::CameraInterface *camera) override {
    // UI
    ImGui::Begin("circe color maps");
    ImGui::Text("Batlow\n");
    drawColormap(batlow);
    ImGui::NewLine();
    ImGui::Text("Matlab Heat\n");
    drawColormap(matlab);
    ImGui::SliderFloat("t", &t, 0.f, 1.f);
    ImGui::End();
    // draw
    cube.program.use();
    cube.program.setUniform("view", camera->getViewTransform());
    cube.program.setUniform("model", cube.transform);
    cube.program.setUniform("projection", camera->getProjectionTransform());
    cube.program.setUniform("color", batlow(t));
    cube.draw();
  }

  static void drawColormap(circe::ColorPalette &palette) {
    ImVec2 base_pos = ImGui::GetCursorScreenPos();
    auto width = ImGui::GetWindowWidth();
    auto draw_list = ImGui::GetWindowDrawList();
    auto step = width / palette.colors.size();
    for (size_t i = 0; i < palette.colors.size(); ++i) {
      auto x = (float) i * step;
      auto color = palette((float) i / palette.colors.size());
      ImGui::SetCursorScreenPos(ImVec2(base_pos.x + x, base_pos.y));
      draw_list->AddRectFilled({base_pos.x + x, base_pos.y},
                               {base_pos.x + x + step, base_pos.y + 40},
                               ImColor(color.r, color.g, color.b, color.a));
    }
    ImGui::SetCursorScreenPos(ImVec2(base_pos.x, base_pos.y + 50));
  }

  circe::gl::SceneModel cube;
  float t{};
  circe::ColorPalette batlow{circe::ColorPalettes::Batlow()};
  circe::ColorPalette matlab{circe::ColorPalettes::MatlabHeatMap()};
};

int main() {
  return ColorMapsExample().run();
}
