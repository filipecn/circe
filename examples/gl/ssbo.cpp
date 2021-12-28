#include <circe/circe.h>

class SSOB : public circe::gl::BaseApp {
public:
  SSOB() : BaseApp(800, 800) {
    hermes::Path shaders_path(std::string(SHADERS_PATH));
    if (!mesh.program.link(shaders_path, "ssbo"))
      HERMES_LOG_ERROR("Failed to load model shader: " + mesh.program.err)
    {  /// setup model
      hermes::AoS aos;
      aos.pushField<hermes::point3>("position");
      aos.resize(4);
      aos.valueAt<hermes::point3>(0, 0) = {0.f, 0.f, 0.f};
      aos.valueAt<hermes::point3>(0, 1) = {0.f, 0.f, 1.f};
      aos.valueAt<hermes::point3>(0, 2) = {0.f, 1.f, 1.f};
      aos.valueAt<hermes::point3>(0, 3) = {0.f, 1.f, 0.f};
      std::vector<i32> indices = {0, 1, 2, 0, 2, 3};
      circe::Model model;
      model = aos;
      model = indices;
      mesh = model;
    }
    {      /// setup SSBO (1 color per triangle)
      hermes::AoS aos;
      aos.pushField<hermes::vec3>("color");
      aos.pushField<f32>("alpha");
      aos.resize(2);
      aos.valueAt<hermes::vec3>(0, 0) = {1.f, 0.f, 0.f};
      aos.valueAt<hermes::vec3>(0, 1) = {0.f, 1.f, 0.f};
      aos.valueAt<f32>(1, 0) = 1.0f;
      aos.valueAt<f32>(1, 1) = 1.0f;
      ssbo = aos;
    }

  }
  void render(circe::CameraInterface *camera) override {
    // update ssbo
    auto m = ssbo.memory()->mapped(GL_MAP_WRITE_BIT);
    f32 f = (this->frame_counter_ % 100) / 100.f;
    ssbo.descriptor.valueAt<hermes::vec3>(m, 0, 0) = {f, f, 1.f};
    ssbo.descriptor.valueAt<f32>(m, 1, 0) = f;
    ssbo.memory()->unmap();

    ssbo.bind();
    mesh.program.use();
    mesh.program.setUniform("view", hermes::transpose(camera->getViewTransform().matrix()));
    mesh.program.setUniform("model", hermes::transpose(mesh.transform.matrix()));
    mesh.program.setUniform("projection", hermes::transpose(camera->getProjectionTransform().matrix()));
    mesh.draw();
  }

  circe::gl::SceneModel mesh;
  circe::gl::ShaderStorageBuffer ssbo;
};

int main() {
  return SSOB().run();
}
