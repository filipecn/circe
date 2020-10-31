#include <circe/circe.h>
#include <sstream>

using namespace circe::gl;

class SceneObjectInteraction : public BaseApp {
public:
  SceneObjectInteraction() : BaseApp(800, 800) {
    grid = std::make_unique<CartesianGrid>(5);
    this->app_->scene.add(grid.get());
    circle_mesh = ponos::RawMeshes::circle();
    mesh = std::make_unique<SceneMeshObject>(circle_mesh.get());
    this->app_->scene.add(mesh.get());
  }

  void render(circe::CameraInterface *camera) override {
    ImGui::Begin("Another Window", &show_another_window);
    std::stringstream ss;
    ss << this->last_FPS_ << "fps" << std::endl;
    ss << "View Matrix\n";
    ss << camera->getViewTransform().matrix() << std::endl;
    ss << "Model Matrix\n";
    ss << camera->getModelTransform().matrix() << std::endl;
    ss << "Projection Matrix\n";
    ss << camera->getProjectionTransform().matrix() << std::endl;
    ss << "MVP Matrix\n";
    ss << camera->getTransform().matrix() << std::endl;
    ss << GraphicsDisplay::instance().getMousePos() << std::endl;
//    ImGui::Text(ss.str().c_str());
    // open Dialog Simple
    if (ImGui::Button("Open File Dialog"))
      igfd::ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", 0, ".");

    // display
    if (igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey"))
    {
      // action if OK
      if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
      {
        std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
        std::string filePath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
        // action
        std::cerr << filePath << std::endl;
      }
      // close
      igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
    }

    ImGui::End();
  }

  bool show_another_window = false;
  std::unique_ptr<CartesianGrid> grid;
  ponos::RawMeshSPtr circle_mesh;
  std::unique_ptr<SceneMeshObject> mesh;
};

int main() {
  SceneObjectInteraction example;
  return example.run();
}