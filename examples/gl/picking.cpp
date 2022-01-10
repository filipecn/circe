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
#include <hermes/random/rng.h>

#define WIDTH 800
#define HEIGHT 800

struct MeshPickingExample {
  MeshPickingExample() {
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    HERMES_ASSERT_WITH_LOG(object.program.link(shaders_path, "picking"), object.program.err)

    auto sphere = circe::Shapes::icosphere(1);
    // The mesh picker requires per face information, so we need to ensure unique index
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

  void pick(const circe::CameraInterface *camera) {
    auto &gd = circe::gl::GraphicsDisplay::instance();
    picker.pick(camera, gd.getMousePos(), [&](const auto &shader) {
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

  void render(circe::CameraInterface *camera) {
    ssbo.bind();
    object.program.use();
    object.program.setUniform("view", camera->getViewTransform());
    object.program.setUniform("model", object.transform);
    object.program.setUniform("projection", camera->getProjectionTransform());
    object.program.setUniform("edge_width", picker.edge_width);
    object.program.setUniform("vertex_width", picker.vertex_width);
    object.draw();
  }

  circe::gl::SceneModel object;
  circe::gl::MeshPicker picker;
  circe::gl::ShaderStorageBuffer ssbo;
};

struct InstancePickingExample {
  InstancePickingExample() {
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    instances.instance_model = circe::Shapes::box(hermes::bbox3::unitBox(true));
    HERMES_ASSERT_WITH_LOG(instances.instance_program.link(shaders_path, "instance"),
                           instances.instance_program.err)
    size_t n = 100;
    instances.resize(n);
    auto instance_data = instances.instanceData();
    circe::ColorPalette palette = circe::HEAT_MATLAB_PALETTE;
    hermes::RNGSampler sampler;
    hermes::HaltonSequence rng;
    for (size_t i = 0; i < n; i++) {
      auto color = palette((1.f * i) / n, 1);
      instance_data.at<circe::Color>("color", i) = color.withAlpha(0.3);
      auto s = rng.randomFloat() * 2 + 0.1;
      auto transform =
          hermes::Transform::translate(hermes::vec3(sampler.sample(
              hermes::bbox3(hermes::point3(-15, -15, -15),
                            hermes::point3(15, 15, 15)))))
              *
                  hermes::Transform::scale(s, s, s);
      instance_data.at<hermes::mat4>("transform_matrix", i) = hermes::transpose(transform.matrix());
    }
    picker.setResolution({WIDTH, HEIGHT});
  }

  void pick(const circe::CameraInterface *camera) {
    static int current_pick{-1};
    auto &gd = circe::gl::GraphicsDisplay::instance();
    picker.pick(camera, gd.getMousePos(), [&](const auto &shader) {
      // TODO it is only working because the "transform_matrix" attribute is
      // TODO in the same location in both shaders (instances and picker)
      instances.instance_model.bind();

      glDrawElementsInstanced(
          instances.instance_model.indexBuffer().element_type,
          instances.instance_model.indexBuffer().element_count * 3,
          instances.instance_model.indexBuffer().data_type,
          nullptr,
          instances.count());
    });

    if (picker.picked_index >= 0 && picker.picked_index < instances.count()) {
      auto instance_data = instances.instanceData();
      if (current_pick != picker.picked_index)
        instance_data.at<circe::Color>("color", current_pick).a = 0.3f;
      instance_data.at<circe::Color>("color", picker.picked_index).a = 1.f;
    } else if (current_pick >= 0) {
      auto instance_data = instances.instanceData();
      instance_data.at<circe::Color>("color", current_pick).a = 0.3f;
    }
    current_pick = picker.picked_index;
  }

  void render(circe::CameraInterface *camera) {
    instances.draw(camera, hermes::Transform());
  }

  circe::gl::InstanceSet instances;
  circe::gl::InstancePicker picker;
};

struct PickingExample : public circe::gl::BaseApp {
  PickingExample() : circe::gl::BaseApp(WIDTH, HEIGHT, "Picking Example") {

  }

  void prepareFrame() override {
    circe::gl::BaseApp::prepareFrame();
    if (mode == 0)
      mesh_picker_example.pick(this->app->getCamera());
    else if (mode == 1)
      instance_picker_example.pick(this->app->getCamera());
  }

  void render(circe::CameraInterface *camera) override {
    ImGui::Begin("Picking");
    ImGui::Combo("mode", &mode, "mesh\0instance\0");
    if (mode == 0) {
      mesh_picker_example.render(camera);
      // ui
      ImGui::Text("Object ID");
      ImGui::Text("%s", hermes::Str::format("{}", mesh_picker_example.picker.picked_index).c_str());
      ImGui::Text("Primitive ID");
      ImGui::Text("%s", hermes::Str::format("{}", mesh_picker_example.picker.picked_primitive_index).c_str());
      ImGui::Text("Edge ID");
      ImGui::Text("%s", hermes::Str::format("{}", mesh_picker_example.picker.picked_edge_index).c_str());
      ImGui::SliderFloat("edge size", &mesh_picker_example.picker.edge_width, 1.f, 5.f);
      ImGui::Text("Vertex ID");
      ImGui::Text("%s", hermes::Str::format("{}", mesh_picker_example.picker.picked_vertex_index).c_str());
      ImGui::SliderFloat("vertex size", &mesh_picker_example.picker.vertex_width, 1.f, 15.f);
    } else if (mode == 1) {
      instance_picker_example.render(camera);
      ImGui::Text("Instance ID");
      ImGui::Text("%s", hermes::Str::format("{}", instance_picker_example.picker.picked_index).c_str());
    }
    ImGui::End();
  }

  int mode{0};
  MeshPickingExample mesh_picker_example;
  InstancePickingExample instance_picker_example;
};

int main() {
  return PickingExample().run();
}


