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
///\file deferred_rendering.cpp.c
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-04-10
///
///\brief

#include "common.h"

#define WIDTH 800
#define HEIGHT 800

class DeferredRenderingExample : public circe::gl::BaseApp {
public:
  DeferredRenderingExample() : BaseApp(WIDTH, HEIGHT, "Deferred Rendering") {
    // resources
    hermes::Path assets_path(std::string(ASSETS_PATH));
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    // scene
    model = circe::Shapes::icosphere({}, 0.2, 5, circe::shape_options::uv | circe::shape_options::normal);
    light_model = circe::Shapes::icosphere({}, 0.02, 2);
    screen_quad = circe::Shapes::box({{-1, -1}, {1, 1}}, circe::shape_options::uv);
    // parameters for g-buffer textures
    circe::gl::Texture::View g_parameters(GL_TEXTURE_2D);
    g_parameters[GL_TEXTURE_MIN_FILTER] = GL_NEAREST;
    g_parameters[GL_TEXTURE_MAG_FILTER] = GL_NEAREST;
    // setup geometry pass framebuffer
    g_framebuffer.resize({WIDTH, HEIGHT});
    g_framebuffer.clear_color = {0, 0, 0, 0};
    // common g-buffer texture attributes
    circe::gl::Texture::Attributes g_attributes = {
        .size_in_texels = {WIDTH, HEIGHT, 1},
        .internal_format = GL_RGBA16F,
        .format = GL_RGBA,
        .type = GL_FLOAT,
        .target = GL_TEXTURE_2D,
    };
    // setup position color buffer
    g_position.set(g_attributes);
    g_position.bind();
    g_parameters.apply();
    g_framebuffer.attachTexture(g_position, GL_COLOR_ATTACHMENT0);
    // setup normal color buffer
    g_normal.set(g_attributes);
    g_normal.bind();
    g_parameters.apply();
    g_framebuffer.attachTexture(g_normal, GL_COLOR_ATTACHMENT1);
    // setup color + specular buffer
    g_attributes.internal_format = GL_RGBA;
    g_albedo_spec.set(g_attributes);
    g_albedo_spec.bind();
    g_parameters.apply();
    g_framebuffer.attachTexture(g_albedo_spec, GL_COLOR_ATTACHMENT2);
    // activate attachments
    g_framebuffer.setOutputBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2});
    // setup shaders
    if (!light_model.program.link(shaders_path, "color"))
      std::cerr << "Failed to compile light model shader: " << light_model.program.err << std::endl;
    if (!g_pass_program.link(shaders_path, "g_pass"))
      std::cerr << "Failed to compile g_pass shader: " << g_pass_program.err << std::endl;
    if (!l_pass_program.link(shaders_path, "l_pass"))
      std::cerr << "Failed to compile l_pass shader: " << l_pass_program.err << std::endl;
    l_pass_program.use();
    l_pass_program.setUniform("gPosition", 0);
    l_pass_program.setUniform("gNormal", 1);
    l_pass_program.setUniform("gAlbedoSpec", 2);
    const unsigned int NR_LIGHTS = 32;
    std::srand(13);
    for (unsigned int i = 0; i < NR_LIGHTS; i++) {
      hermes::point3 position(((rand() % 100) / 100.0) * 5.0 - 2.5,
                             ((rand() % 100) / 100.0) * 5.0 - 2.5,
                             ((rand() % 100) / 100.0) * 5.0 - 2.5);
      hermes::vec3 color(((rand() % 100) / 200.0f) + 0.5,
                        ((rand() % 100) / 200.0f) + 0.5,
                        ((rand() % 100) / 200.0f) + 0.5);
      l_pass_program.setUniform("lights", "Color", i, color);
      l_pass_program.setUniform("lights", "Position", i, position);
      const f32 constant = 1.0;
      const f32 linear = 0.7;
      const f32 quadratic = 1.8;
      // calculate radius of light volume/sphere
      const f32 max_brightness = color.max();
      f32 radius =
          (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * max_brightness)))
              / (2.0f * quadratic);
      l_pass_program.setUniform("lights", "Linear", i, linear);
      l_pass_program.setUniform("lights", "Quadratic", i, quadratic);
      l_pass_program.setUniform("lights", "Radius", i, radius);
      light_positions_.emplace_back(position);
      light_colors_.emplace_back(color);
    }
  }

  void render(circe::CameraInterface *camera) override {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    // geometry pass
    g_framebuffer.render([&]() {
      g_pass_program.use();
      g_pass_program.setUniform("projection", camera->getProjectionTransform());
      g_pass_program.setUniform("view", camera->getViewTransform());
      for (auto i : hermes::Index3Range<i32>(5, 5, 5)) {
        hermes::vec3 v(i.i, i.j, i.k);
        g_pass_program.setUniform("model", hermes::Transform::translate(v - hermes::vec3(2.5, 2.5, 2.5)));
        model.draw();
      }
    });
    // lightning pass
    g_position.bind(GL_TEXTURE0);
    g_normal.bind(GL_TEXTURE1);
    g_albedo_spec.bind(GL_TEXTURE2);
    l_pass_program.use();
    l_pass_program.setUniform("viewPos", camera->getPosition());
    screen_quad.draw();
    // do some forward rendering
    g_framebuffer.blit(GL_DEPTH_BUFFER_BIT);
    light_model.program.use();
    light_model.program.setUniform("projection", camera->getProjectionTransform());
    light_model.program.setUniform("view", camera->getViewTransform());
    for (size_t i = 0; i < light_colors_.size(); ++i) {
      light_model.program.setUniform("model", hermes::Transform::translate(hermes::vec3(light_positions_[i])));
      light_model.program.setUniform("color", circe::Color(light_colors_[i]));
      light_model.draw();
    }

    // show buffer in gui
    ImGui::Begin("G-Buffer");
    g_position_view.render(g_position);
    ImGui::SameLine();
    g_normal_view.render(g_normal);
    g_albedo_view.render(g_albedo_spec);
    ImGui::SameLine();
    g_spec_view.render(g_albedo_spec);
    ImGui::End();
  }
  // g-buffer imgui views
  BufferView g_spec_view{BufferView::componentBufferView()};
  BufferView g_albedo_view{BufferView::vec3BufferView()};
  BufferView g_normal_view{BufferView::vec3BufferView(true)};
  BufferView g_position_view{BufferView::vec3BufferView()};
  // g-buffer textures
  circe::gl::Texture g_position, g_normal, g_albedo_spec;
  // geometry pass framebuffer
  circe::gl::Framebuffer g_framebuffer;
  // geometry pass shader
  circe::gl::Program g_pass_program;
  // lightning pass shader
  circe::gl::Program l_pass_program;
  // scene
  std::vector<hermes::point3> light_positions_;
  std::vector<hermes::vec3> light_colors_;
  circe::gl::SceneModel light_model;
  circe::gl::SceneModel model;
  circe::gl::SceneModel screen_quad;
};

int main() {
  return DeferredRenderingExample().run();
}
