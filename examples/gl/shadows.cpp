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
///\file shadows.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-03-28
///
///\brief

#include <circe/circe.h>
#include "common.h"

/// Light + Shadow Map
struct LightObject {
  void update() {
    light.direction = hermes::vec3(model_transform(hermes::point3(0, 0, 0)));
    shadow_map.setLight(light);
  }

  hermes::Transform model_transform;
  circe::Light light;
  circe::gl::ShadowMap shadow_map;
};

class ShadowsExample : public circe::gl::BaseApp {
public:
  ShadowsExample() : BaseApp(800, 800, "Shadows Example") {
    hermes::Path assets_path(std::string(ASSETS_PATH));
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    // setup lights
    for (auto &light : lights) {
      light.model_transform = hermes::Transform::translate({0, 1, 3});
      light.shadow_map.setLightProjection(hermes::Transform::ortho(-10., 10., -10., 10., 0., 30));
    }
    // setup meshes
    light_box = circe::Shapes::box({{-10, -10, 0}, {10, 10, 30}}, circe::shape_options::wireframe);
    light_model = circe::Shapes::icosphere({}, 0.3, 3);
    ball = circe::Shapes::icosphere({3, 3, 0}, 0.5, 5, circe::shape_options::normal);
    mesh = circe::gl::SceneModel::fromFile("/home/filipecn/Desktop/desk/teapot.obj", circe::shape_options::normal);
    floor = circe::Shapes::plane(hermes::Plane::XY(), {}, {20, 0, 0}, 20,
                                 circe::shape_options::normal);
    wall = circe::Shapes::plane(hermes::Plane::YZ(), {}, {0, 20, 0}, 20,
                                circe::shape_options::normal);
    xy_wall = circe::Shapes::plane(hermes::Plane::XZ(), {}, {0, 0, 20}, 20,
                                   circe::shape_options::normal);
    wall.transform = hermes::Transform::translate({-4, 0, 0});
    xy_wall.transform = hermes::Transform::translate({0, -4, 0});
    // resize floor material
    floor_mtl.albedo = vec3_16(.3f, .0f, .0f);
    floor_mtl.ao = 1;
    floor_mtl.metallic = 0.25;
    floor_mtl.roughness = 0.55;
    // resize mesh material
    mesh_mtl.albedo = vec3_16(.5f, 0.f, 0.f);
    mesh_mtl.ao = 1;
    mesh_mtl.metallic = 0.45;
    mesh_mtl.roughness = 0.35;
    // resize ball material
    ball_mtl.albedo = vec3_16(.0f, 0.f, 0.5f);
    ball_mtl.ao = 1;
    ball_mtl.metallic = 0.45;
    ball_mtl.roughness = 0.35;
    // setup shader program
    if (!light_model.program.link(shaders_path, "color"))
      HERMES_LOG_ERROR("Failed to load model shader: " + light_model.program.err);
    if (!program.link(shaders_path, "pbr"))
      HERMES_LOG_ERROR("Failed to load model shader: " + program.err);
    // setup UBO
    ubo.push(program);
    program.setUniformBlockBinding("PBR", 0);
    // setup SSBO
    hermes::AoS aos;
    aos.pushField<vec3_16>("position");
    aos.pushField<vec3_16>("color");
    aos.resize(1);
    // positions
    aos.valueAt<vec3_16>(0, 0) =
        vec3_16(lights[0].light.direction.x,
                lights[0].light.direction.y,
                lights[0].light.direction.z);
    // colors
    aos.valueAt<vec3_16>(1, 0) = vec3_16(300, 300, 300);
    ssbo = aos;
    ssbo.setBindingIndex(1);
    // controls
    this->app_->keyCallback = [&](int key, int scancode, int action, int modifiers) {
      if (key == GLFW_KEY_ESCAPE)
        this->app_->exit();
      if (key == GLFW_KEY_R)
        operation = ImGuizmo::OPERATION::ROTATE;
      if (key == GLFW_KEY_W)
        operation = ImGuizmo::OPERATION::TRANSLATE;
      if (key == GLFW_KEY_E)
        operation = ImGuizmo::OPERATION::SCALE;
      if (key == GLFW_KEY_T)
        operation = ImGuizmo::OPERATION::BOUNDS;
    };
  }

  void prepareFrame() override {
    circe::gl::BaseApp::prepareFrame();
    ImGuizmo::BeginFrame();
    glDisable(GL_CULL_FACE);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_FRONT);
    for (auto &light: lights)
      light.shadow_map.render([&](const circe::gl::Program &p) {
        p.setUniform("model", xy_wall.transform);
        xy_wall.draw();
        p.setUniform("model", wall.transform);
        wall.draw();
        p.setUniform("model", floor.transform);
        floor.draw();
        p.setUniform("model", mesh.transform);
        mesh.draw();
        p.setUniform("model", ball.transform);
        ball.draw();
      });
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
  }

  void render(circe::CameraInterface *camera) override {
    glEnable(GL_DEPTH_TEST);
    program.use();
    program.setUniform("view", camera->getViewTransform());
    program.setUniform("projection", camera->getProjectionTransform());
    program.setUniform("camPos", camera->getPosition());
    program.setUniform("lightSpaceMatrix", lights[0].shadow_map.light_transform());
    ssbo.bind();
    lights[0].shadow_map.depthMap().bind(GL_TEXTURE0);
    ubo["PBR"] = &floor_mtl;
    program.setUniform("model", floor.transform);
    floor.draw();
    program.setUniform("model", wall.transform);
    wall.draw();
    program.setUniform("model", xy_wall.transform);
    xy_wall.draw();
    ubo["PBR"] = &mesh_mtl;
    program.setUniform("model", mesh.transform);
    mesh.draw();
    ubo["PBR"] = &ball_mtl;
    program.setUniform("model", ball.transform);
    ball.draw();
    // draw lights
    light_model.program.use();
    light_model.program.setUniform("view", camera->getViewTransform());
    light_model.program.setUniform("projection", camera->getProjectionTransform());
    light_model.program.setUniform("color", circe::Color::White());
    light_model.program.setUniform("model", lights[0].model_transform);
    light_model.draw();
    light_model.program.setUniform("model", light_box.transform);
    light_box.draw();

    ImGui::Begin("Shadow Map");
    depth_buffer_view.render(lights[0].shadow_map.depthMap());
    ImGui::End();
    // gizmo
    ImGuizmo::SetRect(0, 0, this->app_->viewports[0].width, this->app_->viewports[0].height);
    circe::Gizmo::update(camera, lights[0].model_transform, operation);

    updateLights();
  }

  void updateLights() {
    auto m = ssbo.memory()->mapped(GL_MAP_WRITE_BIT);
    lights[0].update();
    ssbo.descriptor.valueAt<vec3_16>(m, 0, 0) =
        vec3_16(lights[0].light.direction.x,
                lights[0].light.direction.y,
                lights[0].light.direction.z);
    ssbo.memory()->unmap();
    // update debug box
    auto t = hermes::inverse(hermes::Transform::lookAt(hermes::point3() + lights[0].light.direction));
    light_box.transform = t;
  }

  // imgui
  BufferView depth_buffer_view{BufferView::depthBufferView()};
//  GLuint texture_id{0};
  ImGuizmo::OPERATION operation{ImGuizmo::OPERATION::TRANSLATE};
  // lights
  LightObject lights[4];
  circe::gl::SceneModel light_model, light_box;
  // scene
  circe::gl::SceneModel mesh, ball, floor, wall, xy_wall;
  // shader
  circe::gl::Program program;
  circe::gl::UniformBuffer ubo;
  circe::gl::ShaderStorageBuffer ssbo;
  // materials
  PBR_UB floor_mtl;
  PBR_UB mesh_mtl;
  PBR_UB ball_mtl;
};

int main() {
  return ShadowsExample().run();
}