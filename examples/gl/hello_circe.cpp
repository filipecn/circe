#include <circe/circe.h>
#include "common.h"

class HelloCirce : public circe::gl::BaseApp {
public:
  HelloCirce() : BaseApp(800, 800) {
    // setup object model
    hermes::Path assets_path(std::string(ASSETS_PATH));
    // setup reference grid
    hermes::Path shaders_path(std::string(SHADERS_PATH));
//    mesh = circe::gl::SceneModel::fromFile("/home/filipecn/Desktop/teapot.obj", circe::shape_options::normal);
    mesh = circe::Shapes::icosphere(3, circe::shape_options::normal);
    if (!mesh.program.link(shaders_path, "scene_object"))
      HERMES_LOG_ERROR("Failed to load model shader: " + mesh.program.err);

    /// setup UBO /////////////////////////////////////////////////////////////
    scene_ubo.push(mesh.program);
    mesh.program.setUniformBlockBinding("PBR", 0);

    pbr_ubo_data.albedo = vec3_16(.5f, 0.f, 0.f);
    pbr_ubo_data.ao = 1;
    pbr_ubo_data.metallic = 0.45;
    pbr_ubo_data.roughness = 0.35;
    scene_ubo["PBR"] = &pbr_ubo_data;
    /// setup SSBO ////////////////////////////////////////////////////////////
    hermes::AoS aos;
    aos.pushField<vec3_16>("position");
    aos.pushField<vec3_16>("color");
    aos.resize(4);
    // positions
    aos.valueAt<vec3_16>(0, 0) = vec3_16(10, 10, 10);
    aos.valueAt<vec3_16>(0, 1) = vec3_16(-10, -10, 10);
    aos.valueAt<vec3_16>(0, 2) = vec3_16(-10, 10, 10);
    aos.valueAt<vec3_16>(0, 3) = vec3_16(10, -10, 10);
    // colors
    aos.valueAt<vec3_16>(1, 0) = vec3_16(300, 300, 300);
    aos.valueAt<vec3_16>(1, 1) = vec3_16(300, 300, 300);
    aos.valueAt<vec3_16>(1, 2) = vec3_16(300, 300, 300);
    aos.valueAt<vec3_16>(1, 3) = vec3_16(300, 300, 300);
    scene_ssbo = aos;
    scene_ssbo.setBindingIndex(1);
  }

  void render(circe::CameraInterface *camera) override {
    glEnable(GL_DEPTH_TEST);
    // render model
    mesh.program.use();
    mesh.program.setUniform("view", camera->getViewTransform());
    mesh.program.setUniform("model", mesh.transform);
    mesh.program.setUniform("projection", camera->getProjectionTransform());
    mesh.program.setUniform("camPos", camera->getPosition());
    scene_ssbo.bind();
    mesh.draw();
  }

  // scene
  circe::gl::SceneModel mesh;
  circe::gl::UniformBuffer scene_ubo;
  circe::gl::ShaderStorageBuffer scene_ssbo;

 PBR_UB pbr_ubo_data;
};

int main() {
  return HelloCirce().run();
}
