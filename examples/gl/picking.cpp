/// Copyright (c) 2021, FilipeCN.
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
///\file picking.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-12-13
///
///\brief

#include "common.h"

#define WIDTH 800
#define HEIGHT 800

struct PickingExample : public circe::gl::BaseApp {
  PickingExample() : circe::gl::BaseApp(WIDTH, HEIGHT, "Picking Example") {
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    if (!object.program.link(shaders_path, "picking"))
      HERMES_LOG_ERROR("Failed to load model shader: " + object.program.err)

    auto sphere = circe::Shapes::icosphere(1);
    // Picker requires per face information, so we need to ensure unique index
    // positions, i.e. a single vertex for each index
    // Also, we need just the position attribute
    auto model = circe::Shapes::convert(sphere, circe::shape_options::unique_positions);
    // push barycentric coordinates attribute
    auto bid = model.pushAttribute<hermes::vec3>("barycentric");
    // fill
    auto barycentric = model.attributeAccessor<hermes::vec3>(bid);
    for (int f = 0; f < model.elementCount(); ++f) {
      barycentric[f * 3 + 0] = {1, 0, 0};
      barycentric[f * 3 + 1] = {0, 1, 0};
      barycentric[f * 3 + 2] = {0, 0, 1};
    }
    object = model;
    picker.setResolution({WIDTH, HEIGHT});
    {
      // for visualization purposes, lets store edge and vertex selection states
      hermes::AoS aos;
      aos.pushField<u32>("face");
      // I will use one int to store both information like this: edge_index * 10 + vertex_index
      aos.pushField<u32>("edge_vertex");
      aos.resize(object.model().elementCount());
      ssbo = aos;
    }
  }

  void prepareFrame(const circe::gl::ViewportDisplay &display) override {
    BaseApp::prepareFrame(display);
    // pick logic
    auto &gd = circe::gl::GraphicsDisplay::instance();
    picker.pick(display.camera.get(), gd.getMousePos(), [&](const auto &shader) {
      shader.setUniform("object_id", 1);
      shader.setUniform("model", object.transform);
      object.draw();
    });
    // update vertex and edge states
    auto m = ssbo.memory()->mapped(GL_MAP_WRITE_BIT);
    std::memset(m, 0, ssbo.dataSizeInBytes());
    ssbo.descriptor.valueAt<i32>(m, 0, picker.picked_primitive_index) = 1;
    ssbo.descriptor.valueAt<i32>(m, 1, picker.picked_primitive_index) =
        picker.picked_edge_index * 10 + picker.picked_vertex_index;
    ssbo.memory()->unmap();
  }

  void render(circe::CameraInterface *camera) override {
    // usual render
    ssbo.bind();
    object.program.use();
    object.program.setUniform("view", camera->getViewTransform());
    object.program.setUniform("model", object.transform);
    object.program.setUniform("projection", camera->getProjectionTransform());
    object.program.setUniform("edge_width", picker.edge_width);
    object.program.setUniform("vertex_width", picker.vertex_width);
    object.draw();

    // ui
    ImGui::Begin("Picking");
    ImGui::Text("Object ID");
    ImGui::Text("%s", hermes::Str::format("{}", picker.picked_index).c_str());
    ImGui::Text("Primitive ID");
    ImGui::Text("%s", hermes::Str::format("{}", picker.picked_primitive_index).c_str());
    ImGui::Text("Edge ID");
    ImGui::Text("%s", hermes::Str::format("{}", picker.picked_edge_index).c_str());
    ImGui::SliderFloat("edge size", &picker.edge_width, 1.f, 5.f);
    ImGui::Text("Vertex ID");
    ImGui::Text("%s", hermes::Str::format("{}", picker.picked_vertex_index).c_str());
    ImGui::SliderFloat("vertex size", &picker.vertex_width, 1.f, 15.f);
    ImGui::End();
  }

  circe::gl::SceneModel object;
  circe::gl::MeshPicker picker;
  circe::gl::ShaderStorageBuffer ssbo;
};

int main() {
  return PickingExample().run();
}


