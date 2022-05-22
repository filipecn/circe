#include <circe/circe.h>
#include <hermes/random/rng.h>

#include <memory>

using namespace circe::gl;

class InstancesApp : public BaseApp {
public:
  enum MeshType : int {
    Sphere = 0,
    Cube = 1,
    Quad = 2,
    Mesh = 3,
    None = 4,
  };

  InstancesApp() : BaseApp(800, 800, "") {
    // setup resources
    circe::shape_options model_options = circe::shape_options::uv;
    // the manager sets 0 index to sphere model
    HERMES_ASSERT(SceneResourceManager::pushModel(circe::Shapes::icosphere(2, model_options)));
    // the manager sets 1 index to sphere model
    HERMES_ASSERT(SceneResourceManager::pushModel(circe::Shapes::box(hermes::bbox3::unitBox(true), model_options)));
    // the manager sets 2 index to sphere model
    HERMES_ASSERT(SceneResourceManager::pushModel(circe::Shapes::plane(hermes::Plane::XZ(),
                                                                       {},
                                                                       {1, 0, 0},
                                                                       1,
                                                                       model_options)));
    // the manager sets 3 index to sphere model
    HERMES_ASSERT(SceneResourceManager::pushModel(hermes::Path(std::string(ASSETS_PATH)) / "teapot.obj",
                                                  model_options));

    // setup shader
    ProgramManager::setShaderSearchPath(std::string(SHADERS_PATH));
    auto instance_program = ProgramManager::push("instance");
    HERMES_ASSERT(instance_program);
    instance_set.program_handle = *instance_program;

    // setup UI
    object_type_ui.push("Sphere", MeshType::Sphere);
    object_type_ui.push("Cube", MeshType::Cube);
    object_type_ui.push("Quad", MeshType::Quad);
    object_type_ui.push("Mesh", MeshType::Mesh);

    updateMesh(MeshType::Cube);
  }

  void render(circe::CameraInterface *camera) override {
    ImGui::Begin("Controls");
    object_type_ui.draw([&](auto choice) {
      updateMesh(choice);
    });

    std::stringstream ss;
    ss << this->last_FPS_ << "fps" << std::endl;
    ImGui::Text("%s", ss.str().c_str());

//    if (auto obj = circe::ImguiOpenDialog::file_dialog_button("Pick obj", ".obj"))
//      obj_path = obj.path;

    instance_set.draw(camera, hermes::Transform());

    ImGui::End();
  }

  void resize(u64 n) {
    instance_set.resize(n);
    auto instance_data = instance_set.instanceData();
    // setup instance properties
    circe::ColorPalette palette = circe::ColorPalettes::Batlow();
    hermes::RNGSampler sampler;
    hermes::HaltonSequence rng;
    for (size_t i = 0; i < n; i++) {
      auto color = palette((1.f * i) / n, 1);
      instance_data.at<circe::Color>("color", i) = color;
      auto s = rng.randomFloat() * 2 + 0.1;
      auto transform =
          hermes::Transform::translate(hermes::vec3(sampler.sample(
              hermes::bbox3(hermes::point3(-5, -5, -5),
                            hermes::point3(5, 5, 5)))))
              *
                  hermes::Transform::scale(s, s, s);
      instance_data.at<hermes::mat4>("transform_matrix", i) = hermes::transpose(transform.matrix());
    }
    //    HERMES_LOG_VARIABLE(instance_data.memoryDump())
  }

  void updateMesh(MeshType type) {
    static MeshType last_type = MeshType::None;
    if (type != last_type) {
      last_type = type;
      instance_set.model_handle = SceneResourceManager::modelHandle((int) type).value();
      resize(100);
    }
  }

  // scene
  InstanceSet instance_set;

  // UI
  circe::ImGuiRadioButtonSet<MeshType> object_type_ui;
};

int main() {
  return InstancesApp().run();
}
