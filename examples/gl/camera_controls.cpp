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
///\file camera_controls.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-02-03
///
///\brief

#include <circe/circe.h>

hermes::Path shaders_path;

struct CameraControlsExample : public circe::gl::BaseApp {
  CameraControlsExample() : circe::gl::BaseApp(1600, 800, "Camera Controls", circe::app_options::no_viewport) {
    // setup 2 viewports_
    // the first will show the camera view
    app->addViewport3D(0, 0, 800, 800,
                     [&](auto *camera) {
                       renderScene(camera);
                     });
    // the second will show camera ui from another point of view
    app->addViewport3D(1200, 400, 400, 400,
                     [&](auto *camera) {
                       renderScene(camera);
                       renderCamera(camera);
                     });
    app->viewport(1).camera().setUpVector({0, 0, 1});
    app->viewport(1).camera().setPosition({-10, -10, 10});
    HERMES_ASSERT_WITH_LOG(view_plane.program.link(shaders_path, "color"), view_plane.program.err);
//    hyperboloid = circe::Shapes::hyperboloid(1, 1, 1, -1, circe::shape_options::vertices);
    hyperboloid =
        circe::Shapes::fromSurfaceSamples({{-1, -1}, {1, 1}}, {100, 100},
                                          [](const hermes::point2 &p) -> real_t {
                                            const float r = 0.5;
                                            const float r2 = r * r;
                                            const float x2y2 = p.x * p.x + p.y * p.y;
                                            if (x2y2 <= r2 * 0.5)
                                              return std::sqrt(r2 - x2y2);
                                            return r2 / (2 * std::sqrt(x2y2));
                                          }, circe::shape_options::vertices);
  }

  void prepareFrame() override {
    BaseApp::prepareFrame();
    camera_model.update(&app->viewport().camera());
    // update view plane
    auto center = app->viewport().camera().getTarget();
    auto vp = app->viewport().camera().viewPlane(center);
    auto pn = hermes::vec3(vp.normal).normalized();
    auto pt = hermes::vec3(1, 0, 0);
    if (std::fabs(hermes::dot(pt, pn)) > 0.99999)
      pt = {0, 1, 0};
    pt = hermes::cross(pn, pt).normalized();
    view_plane = circe::Shapes::plane(vp, center, pt, 1, circe::shape_options::wireframe);
  }

  void renderScene(circe::CameraInterface *camera) {
    c_grid.draw(camera);
  }

  void renderCamera(circe::CameraInterface *camera) {
    camera_model.draw(camera);
//    tb_model.draw(app->getCamera()->trackball, camera);

    view_plane.program.use();
    view_plane.program.setUniform("projection", camera->getProjectionTransform());
    view_plane.program.setUniform("view", camera->getViewTransform());
    view_plane.program.setUniform("model", hermes::Transform());
    view_plane.program.setUniform("color", circe::Color::Gray(0.1));
    view_plane.draw();
    hyperboloid.draw();

//    auto tb = app->viewport(0).camera()->trackball;
//    if (tb.currentModeType() == circe::TrackballInterface::Mode::ROTATE) {
//      axis.color = circe::Color::Green();
//      axis.draw(reinterpret_cast<const circe::RotateMode *>(tb.currentMode())->d_axis.normalized(), camera);
//      axis.color = circe::Color::Black();
//      axis.color = circe::Color::Yellow();
//      axis.draw(reinterpret_cast<const circe::RotateMode *>(tb.currentMode())->d_screen_axis, camera);
//      axis.draw(reinterpret_cast<const circe::RotateMode *>(tb.currentMode())->d_a, camera);
//      axis.color = circe::Color::Yellow();
//      axis.draw(reinterpret_cast<const circe::RotateMode *>(tb.currentMode())->d_b, camera);
//      axis.draw(reinterpret_cast<const circe::RotateMode *>(tb.currentMode())->d_axis, camera);
//    }
    ImGui::SetNextWindowSize({400, 400});
    ImGui::SetNextWindowPos({1200, 400});
    ImGui::Begin("controls");
//    if (tb.currentModeType() == circe::TrackballInterface::Mode::ROTATE) {
//      ImGui::Text("%s",
//                  hermes::Str::format("{}",
//                                      reinterpret_cast<const circe::RotateMode *>(tb.currentMode())->d_b).c_str());
//      ImGui::Text("%s",
//                  hermes::Str::format("{}",
//                                      reinterpret_cast<const circe::RotateMode *>(tb.currentMode())->mouse_pos).c_str());
//
//      ImGui::Text("ALIGN");
//      auto tt =
//          hermes::Transform::alignVectors({1, 0, 0},
//                                          reinterpret_cast<const circe::RotateMode *>(tb.currentMode())->d_b);
//      ImGui::Text("%s",
//                  hermes::Str::format("{}",
//                                      reinterpret_cast<const circe::RotateMode *>(tb.currentMode())->d_b.normalized()).c_str());
//      ImGui::Text("%s",
//                  hermes::Str::format("{}", tt(hermes::vec3(1, 0, 0)).normalized()).c_str());
//      ImGui::Text("%s",
//                  hermes::Str::format("{}", tt(hermes::vec3(0, 1, 0)).normalized()).c_str());
//      ImGui::Text("%s",
//                  hermes::Str::format("{}", tt(hermes::vec3(0, 0, 1)).normalized()).c_str());
//    }
    ImGui::End();
  }

  // scene
  circe::gl::helpers::CartesianGrid c_grid;
  circe::gl::helpers::CameraModel camera_model;
  circe::gl::helpers::TrackballInterfaceModel tb_model;
  // debug
  circe::gl::SceneModel view_plane;
  circe::gl::SceneModel hyperboloid;
  circe::gl::helpers::VectorModel axis;
};

int main() {
  shaders_path = hermes::Path(std::string(SHADERS_PATH));
  return CameraControlsExample().run();
}