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
    hermes::Path assets_path(std::string(ASSETS_PATH));
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    // setup skybox
    skybox = circe::Shapes::box({{-1, -1, -1}, {1, 1, 1}});
    model = circe::Shapes::box({{-1, -1, -1}, {1, 1, 1}}, circe::shape_options::normal);
    // textures
    cubemap = circe::gl::Texture::fromFiles({
                                                "/home/filipecn/Desktop/desk/skybox/right.jpg",
                                                "/home/filipecn/Desktop/desk/skybox/left.jpg",
                                                "/home/filipecn/Desktop/desk/skybox/top.jpg",
                                                "/home/filipecn/Desktop/desk/skybox/bottom.jpg",
                                                "/home/filipecn/Desktop/desk/skybox/front.jpg",
                                                "/home/filipecn/Desktop/desk/skybox/back.jpg",
                                            });

    cubemap.bind();
    circe::gl::Texture::View view(GL_TEXTURE_CUBE_MAP);
    view.apply();
    cubemap.unbind();

    unfolded = circe::gl::Texture::fromTexture(cubemap);

    if (!skybox.program.link(shaders_path, "skybox"))
      HERMES_LOG_ERROR("Failed to load model shader: " + skybox.program.err);
    if (!model.program.link(shaders_path, "env_map"))
      HERMES_LOG_ERROR("Failed to load model shader: " + model.program.err);
  }

  void prepareFrame(const circe::gl::ViewportDisplay &display) override {
    circe::gl::BaseApp::prepareFrame(display);
    ImGuizmo::BeginFrame();
  }

  void render(circe::CameraInterface *camera) override {
    glEnable(GL_DEPTH_TEST);
    cubemap.bind(GL_TEXTURE0);
    model.program.use();
    model.program.setUniform("projection", camera->getProjectionTransform());
    model.program.setUniform("view", camera->getViewTransform());
    model.program.setUniform("model", model.transform);
    model.program.setUniform("skybox", 0);
    model.program.setUniform("cameraPos", camera->getPosition());
    model.draw();
    glDepthFunc(GL_LEQUAL);
    skybox.program.use();
    auto m = camera->getViewTransform().matrix();
    m[0][3] = m[1][3] = m[2][3] = 0;
    skybox.program.setUniform("projection", camera->getProjectionTransform());
    skybox.program.setUniform("view", hermes::transpose(m));
    skybox.program.setUniform("skybox", 0);
    skybox.draw();
    glDepthFunc(GL_LESS);

    ImGui::Begin("Cubemap");
    unfolded.bind(GL_TEXTURE0);
    auto texture_id = unfolded.textureObjectId();
    ImGui::Image((void *) (intptr_t) (texture_id),
                 {static_cast<float>(unfolded.size().width / 2), static_cast<float>(unfolded.size().height / 2)},
                 {0, 1}, {1, 0});
    ImGui::End();
    // gizmo
    ImGuizmo::SetRect(0, 0, this->app_->viewports[0].width, this->app_->viewports[0].height);
    hermes::Transform t;
    circe::Gizmo::update(camera, t, ImGuizmo::OPERATION::TRANSLATE);
  }

  circe::gl::SceneModel model;
  circe::gl::SceneModel skybox;
  circe::gl::Texture cubemap, unfolded;
};

int main() {
  return SkyboxExample().run();
}