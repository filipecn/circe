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
///\file skybox.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-04-06
///
///\brief Example of a skybox with enviroment mapping

#include <circe/circe.h>

class SkyboxExample : public circe::gl::BaseApp {
public:
  SkyboxExample() : BaseApp(800, 800) {
    // resources
    ponos::Path assets_path(std::string(ASSETS_PATH));
    ponos::Path shaders_path(std::string(SHADERS_PATH));
    // setup skybox
    skybox = circe::Shapes::box({{-1, -1, -1}, {1, 1, 1}});
    model = circe::Shapes::box({{-1, -1, -1}, {1, 1, 1}}, circe::shape_options::normal);
    // load cube map
    cubemap = circe::gl::Texture::fromFiles({
                                                "/home/filipecn/Desktop/skybox/right.jpg",
                                                "/home/filipecn/Desktop/skybox/left.jpg",
                                                "/home/filipecn/Desktop/skybox/top.jpg",
                                                "/home/filipecn/Desktop/skybox/bottom.jpg",
                                                "/home/filipecn/Desktop/skybox/front.jpg",
                                                "/home/filipecn/Desktop/skybox/back.jpg",
                                            });
    cubemap.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    cubemap.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    cubemap.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    cubemap.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    cubemap.setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    if (!skybox.program.link(shaders_path, "skybox"))
      spdlog::error("Failed to load model shader: " + skybox.program.err);
    if (!model.program.link(shaders_path, "env_map"))
      spdlog::error("Failed to load model shader: " + model.program.err);
  }

  void render(circe::CameraInterface *camera) override {
    glEnable(GL_DEPTH_TEST);
    cubemap.bind(GL_TEXTURE0);

    model.program.use();
    model.program.setUniform("projection", camera->getProjectionTransform());
    model.program.setUniform("view", camera->getViewTransform());
    model.program.setUniform("model", model.transform);
    model.program.setUniform("cameraPos", camera->getPosition());
    model.draw();
    glDepthFunc(GL_LEQUAL);
    skybox.program.use();
    auto m = camera->getViewTransform().matrix();
    m[0][3] = m[1][3] = m[2][3] = 0;
    skybox.program.setUniform("projection", camera->getProjectionTransform());
    skybox.program.setUniform("view", ponos::transpose(m));
    skybox.program.setUniform("skybox", 0);
    skybox.draw();
    glDepthFunc(GL_LESS);
  }

  circe::gl::SceneModel model;
  circe::gl::SceneModel skybox;
  circe::gl::Texture cubemap;
};

int main() {
  return SkyboxExample().run();
}