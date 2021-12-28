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
///\file volume_texture.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-12-21
///
///\brief

#include <circe/circe.h>

float getSpiral(const hermes::point3 &p, float out_value) {
  const float r = 0.5f * (0.5f - fabsf(p.y));
  const float a = (float) (hermes::Constants::pi * 8.0) * p.y;
  const float dx = (cosf(a) * r - p.x) * 2.0f;
  const float dy = (sinf(a) * r - p.z) * 2.0f;
  return powf(fmaxf((1.0f - dx * dx - dy * dy), 0.0f), 8.0f) *
      out_value;
}

float getMenger(const hermes::point3 &p, float out_value) {
  hermes::point3 pos = p + hermes::vec3(0.5f, 0.5f, 0.5f);
  const unsigned int steps = 3;
  for (unsigned int i = 0; i < steps; ++i) {
    pos *= 3.0f;
    const int s = ((int) pos.x & 1) + ((int) pos.y & 1) + ((int) pos.z & 1);
    if (s >= 2)
      return 0.0f;
  }
  return out_value;
}

struct VolumeTextureExample : public circe::gl::BaseApp {
  VolumeTextureExample() : circe::gl::BaseApp(1600, 800, "Volume Texture Example") {

    hermes::Path shaders_path(std::string(SHADERS_PATH));

    // domain transform
    auto domain_box = hermes::bbox3::unitBox();
    // wireframe
    w_box_model = circe::Shapes::box(domain_box, circe::shape_options::wireframe);
    if (!w_box_model.program.link(shaders_path, "color")) {
      HERMES_LOG_ERROR("Failed to load model shader: " + w_box_model.program.err)
      exit(-1);
    }
    w_box_model.program.use();
    w_box_model.program.setUniform("color", circe::Color::Black());

    // init box model and volume texture shader
    box_model = circe::Shapes::box(domain_box, circe::shape_options::uvw);
    if (!box_model.program.link(shaders_path, "woodcock_volume")) {
      HERMES_LOG_ERROR("Failed to load model shader: " + box_model.program.err)
      exit(-1);
    }

    // init density texture
    circe::gl::Texture::Attributes attributes = {
        .size_in_texels = volume_resolution,
        .internal_format = GL_RED,
        .format = GL_RED,
        .type = GL_FLOAT,
        .target = GL_TEXTURE_3D,
    };
    t_density.set(attributes);
    circe::gl::Texture::View parameters(GL_TEXTURE_3D);
    t_density.bind();
    parameters.apply();
    // set density data
    std::vector<float> density(volume_resolution.total(), 0.f);
    hermes::range3 index_range(volume_resolution);
    auto domain_transform =
        hermes::Transform::translate(hermes::vec3(-0.5)) * hermes::Transform::scale(1.f / volume_resolution.width,
                                                                                    1.f / volume_resolution.height,
                                                                                    1.f / volume_resolution.depth);
    for (auto ijk :  index_range)
      density[index_range.flatIndex(ijk)] = getMenger(domain_transform(ijk), 0);
    t_density.setTexels(density.data());

    this->app_->viewports[0].clear_screen_color = circe::Color::Black();
  }

  void prepareFrame(const circe::gl::ViewportDisplay &display) override {
    circe::gl::BaseApp::prepareFrame(display);
    ImGuizmo::BeginFrame();
  }

  void render(circe::CameraInterface *camera) override {
    t_density.bind(GL_TEXTURE0);
    HERMES_ASSERT(box_model.program.use())
    box_model.program.setUniform("projection", camera->getProjectionTransform());
    box_model.program.setUniform("model", camera->getModelTransform());
    box_model.program.setUniform("view", camera->getViewTransform());
    box_model.program.setUniform("extinction_tex", 0);
    box_model.program.setUniform("albedo", albedo);
    box_model.program.setUniform("max_iterations", max_iterations);
    box_model.program.setUniform("max_interactions", max_interactions);
    box_model.program.setUniform("max_extinction", max_extinction);

    box_model.program.setUniform("domain_box_lower", hermes::point3(0, 0, 0));
    box_model.program.setUniform("domain_box_upper", hermes::point3(1, 1, 1));

    box_model.program.setUniform("camera.pos", camera->getPosition());
    box_model.program.setUniform("camera.up", hermes::normalize(camera->getUpVector()));
//    box_model.program.setUniform("camera.dir", hermes::normalize(camera->getDirection()));
    box_model.program.setUniform("camera.right", hermes::normalize(camera->getRight()));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    box_model.draw();

    w_box_model.program.use();
    w_box_model.program.setUniform("projection", camera->getProjectionTransform());
    w_box_model.program.setUniform("model", camera->getModelTransform());
    w_box_model.program.setUniform("view", camera->getViewTransform());
    w_box_model.program.setUniform("color", circe::Color::White());

    w_box_model.draw();

    // UI
    ImGui::Begin("controls");
    ImGui::InputInt("iterations", &max_iterations);
    ImGui::InputInt("interactions", &max_interactions);
    ImGui::SliderFloat("albedo", &albedo, 0, 1);
    ImGui::InputFloat("extinction", &max_extinction);
    ImGui::End();

    // gizmo
    ImGuizmo::SetRect(0, 0, this->app_->viewports[0].width, this->app_->viewports[0].height);
    circe::Gizmo::update(camera, gizmo_transform, ImGuizmo::OPERATION::TRANSLATE);
  }

  // parameters
  int max_iterations{2};
  int max_interactions{1024};
  float max_extinction{1.f};
  float albedo{0.8f};

  // scene
  hermes::Transform gizmo_transform;
  hermes::size3 volume_resolution{128, 128, 128};
  circe::gl::Texture t_density;
  circe::gl::SceneModel box_model;
  circe::gl::SceneModel w_box_model;
};

int main() {
  return VolumeTextureExample().run();
}
