#include <circe/circe.h>
#include <hermes/random/rng.h>

#include <memory>

using namespace circe::gl;

class InstancesApp : public BaseApp {
public:
  enum MeshType {
    None,
    Sphere,
    Cube,
    Circle,
    Quad
  };

  InstancesApp() : BaseApp(800, 800) {
    // resources
    hermes::Path assets_path(std::string(ASSETS_PATH));
    hermes::Path shaders_path(std::string(SHADERS_PATH));

    // instance shader
    if (!instance_set.instance_program.link(shaders_path, "instance"))
      HERMES_LOG_ERROR("Failed to load instance shader: " + instance_set.instance_program.err);

    updateMesh(MeshType::Cube);

    this->app_->scene.add(&instance_set);
  }

  void render(circe::CameraInterface *camera) override {
    ImGui::Begin("Controls");
    static MeshType mesh_choice = MeshType::None;
    if (ImGui::RadioButton("Sphere", mesh_choice == Sphere))
      mesh_choice = Sphere;
    if (ImGui::RadioButton("Cube", mesh_choice == Cube))
      mesh_choice = Cube;
    if (ImGui::RadioButton("Circle", mesh_choice == Circle))
      mesh_choice = Circle;
    if (ImGui::RadioButton("Quad", mesh_choice == Quad))
      mesh_choice = Quad;
    updateMesh(mesh_choice);
    std::stringstream ss;
    ss << this->last_FPS_ << "fps" << std::endl;
    ImGui::Text(ss.str().c_str());

    auto obj = circe::ImguiFolderDialog::folder_dialog_button("Pick obj");
    if (obj)
      HERMES_LOG_VARIABLE(obj.path)


    // open Dialog Simple
    if (ImGui::Button("Open File Dialog"))
      igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");

    // display
    if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey")) {
      // action if OK
      if (igfd::ImGuiFileDialog::Instance()->IsOk == true) {
        std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
        std::string filePath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
        // action
        std::cerr << filePath << std::endl;
      }
      // close
      igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
    }

    instance_set.draw(camera, hermes::Transform());

    ImGui::End();
  }

  void resize(u64 n) {
    instance_set.resize(n);
    auto instance_data = instance_set.instanceData();
    // setup instance properties
    circe::ColorPalette palette = circe::HEAT_MATLAB_PALETTE;
    hermes::RNGSampler sampler;
    hermes::HaltonSequence rng;
    for (size_t i = 0; i < n; i++) {
      auto color = palette((1.f * i) / n, 1);
      instance_data.at<circe::Color>("color", i) = color;
      auto s = rng.randomFloat() * 2 + 0.1;
      auto transform =
          hermes::transpose((hermes::Transform::scale(s, s, s) *
              hermes::Transform::translate(hermes::vec3(sampler.sample(
                  hermes::bbox3(hermes::point3(-5, -5, -5),
                                hermes::point3(5, 5, 5)))))).matrix());
      instance_data.at<hermes::mat4>("transform_matrix", i) = transform;
    }
    //    HERMES_LOG_VARIABLE(instance_data.memoryDump())
  }

  void updateMesh(MeshType type) {
    static MeshType last_type = MeshType::None;
    if (type != last_type) {
      last_type = type;
      SceneModel model;
      switch (type) {
      case MeshType::
        Sphere:model = circe::Shapes::icosphere(2);
        instance_set.instance_model = std::move(model);
        break;
      case MeshType::Circle:
      case MeshType::Cube:model = circe::Shapes::box(hermes::bbox3::unitBox());
        instance_set.instance_model = std::move(model);
        break;
      case MeshType::Quad:
      default:HERMES_NOT_IMPLEMENTED
      }

      resize(100);
    }
  }

  // scene
  InstanceSet instance_set;
};

int main() {
  return InstancesApp().run();
}
