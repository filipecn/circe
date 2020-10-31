#include <circe/circe.h>

#include <memory>

using namespace circe::gl;

class InstancesApp : public BaseApp {
public:
  enum MeshType {
    None,
    Sphere,
    Circle,
    Quad
  };

  InstancesApp() : BaseApp(800, 800) {
    // setup instance shader
    const char *fs = CIRCE_INSTANCES_FS;
    const char *vs = CIRCE_INSTANCES_VS;
    instance_shader = createShaderProgramPtr(vs, nullptr, fs);
    instance_shader->addVertexAttribute("position", 0);
    instance_shader->addVertexAttribute("color", 1);
    instance_shader->addVertexAttribute("transform_matrix", 2);
    instance_shader->addUniform("model_view_matrix", 3);
    instance_shader->addUniform("projection_matrix", 4);
    instance_set.setInstanceShader(*instance_shader);
    // create instance attributes
    BufferDescriptor trans = BufferDescriptor::forArrayStream(16);
    trans.addAttribute("transform_matrix", 16, 0, trans.data_type);
    tid = instance_set.add(trans);
    // create a buffer for particles colors
    circe::gl::BufferDescriptor col =
        circe::gl::BufferDescriptor::forArrayStream(4);  // r g b a
    col.addAttribute("color", 4, 0, col.data_type); // 4 -> r g b a
    colid = instance_set.add(col);
    // setup scene
    this->app_->scene.add(&instance_set);
    grid = CartesianGrid(5);
    this->app_->scene.add(&grid);
  }

  void render(circe::CameraInterface *camera) override {
    ImGui::Begin("Controls");
    static MeshType mesh_choice = MeshType::None;
    if (ImGui::RadioButton("Sphere", mesh_choice == Sphere))
      mesh_choice = Sphere;
    if (ImGui::RadioButton("Circle", mesh_choice == Circle))
      mesh_choice = Circle;
    if (ImGui::RadioButton("Quad", mesh_choice == Quad))
      mesh_choice = Quad;
    updateMesh(mesh_choice);

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

    ImGui::End();
  }

  void resize(u64 n) {
    instance_set.resize(n);
    circe::ColorPalette palette = circe::HEAT_MATLAB_PALETTE;
    ponos::RNGSampler sampler;
    ponos::HaltonSequence rng;
    for (size_t i = 0; i < n; i++) {
      auto color = palette((1.f * i) / n, 1.f);
      auto c = instance_set.instanceF(colid, i);
      c[0] = color.r;
      c[1] = color.g;
      c[2] = color.b;
      c[3] = color.a;
      c[3] = 0.4;
      auto m = instance_set.instanceF(tid, i);
      float t[16];
      (ponos::scale(1, 1, 1) *
          ponos::translate(ponos::vec3(sampler.sample(
              ponos::bbox3(ponos::point3(-5, 0, 0), ponos::point3(5, 5, 5))))))
          .matrix()
          .column_major(t);
      for (size_t k = 0; k < 16; k++)
        m[k] = t[k];
    }
  }

  void updateMesh(MeshType type) {
    static MeshType last_type = MeshType::None;
    if (type != last_type) {
      last_type = type;
//      switch (type) {
//      case MeshType::Sphere:
        instance_mesh = ponos::RawMeshSPtr(
            ponos::create_icosphere_mesh(ponos::point3(), 1.f, 0, false, false));
//        break;
//      }
      instance_scene_mesh = std::make_unique<SceneMesh>(instance_mesh.get());
      instance_set.setInstanceMesh(instance_scene_mesh.get());
      resize(100);
    }
  }

  // instance
  ponos::RawMeshSPtr instance_mesh;
  ShaderProgramPtr instance_shader;
  std::unique_ptr<SceneMesh> instance_scene_mesh;
  // instance set
  InstanceSet instance_set;
  u32 tid, colid; // instance attribute pointers for transform and color
  // scene
  CartesianGrid grid;
};

int main() {
  InstancesApp instances_app;
  return instances_app.run();
  circe::gl::SceneApp<> app(800, 800);
  std::shared_ptr<circe::gl::InstanceSet> spheres, quads;
  // generate a bunch of random quads
  // but now each instance has a transform matrix
  size_t n = 400;
  // generate base mesh
  ponos::RawMeshSPtr sphereMesh(
      ponos::create_icosphere_mesh(ponos::point3(), 1.f, 0, false, false));
  ponos::RawMeshSPtr quadMesh(ponos::create_quad_mesh(
      ponos::point3(0, 0, 0), ponos::point3(1, 0, 0), ponos::point3(1, 1, 0),
      ponos::point3(0, 1, 0), false, false));
  ponos::RawMeshSPtr wquadMesh(ponos::create_quad_wireframe_mesh(
      ponos::point3(0, 0, 0), ponos::point3(1, 0, 0), ponos::point3(1, 1, 0),
      ponos::point3(0, 1, 0)));
  // ponos::RawMeshSPtr circleMesh(ponos::RawMeshes::icosphere());
  ponos::RawMeshSPtr segmentMesh(
//      ponos::RawMeshes::segment(ponos::point2(1, 0)));
//      ponos::RawMeshes::circle());
      ponos::RawMeshes::icosphere(ponos::point3(), 1, 5, true, false));
  ponos::RawMeshSPtr cube(ponos::RawMeshes::cube());
  // circe::SceneMesh qm(*wquadMesh.get());
  circe::gl::SceneMesh qm(segmentMesh.get());
  const char *fs = CIRCE_INSTANCES_FS;
  const char *vs = CIRCE_INSTANCES_VS;
  circe::gl::ShaderProgram quadShader(vs, nullptr, fs);
  quadShader.addVertexAttribute("position", 0);
  quadShader.addVertexAttribute("color", 1);
  quadShader.addVertexAttribute("transform_matrix", 2);
  quadShader.addUniform("model_view_matrix", 3);
  quadShader.addUniform("projection_matrix", 4);
  quads = std::make_shared<circe::gl::InstanceSet>(&qm, quadShader, n / 2);
  {
    // create a buffer for particles positions + sizes
    circe::gl::BufferDescriptor trans = circe::gl::BufferDescriptor::forArrayStream(16);
    trans.addAttribute("transform_matrix", 16, 0, trans.data_type);
    uint tid = quads->add(trans);
    // create a buffer for particles colors
    circe::gl::BufferDescriptor col =
        circe::gl::BufferDescriptor::forArrayStream(4);  // r g b a
    col.addAttribute("color", 4, 0, col.data_type); // 4 -> r g b a
    uint colid = quads->add(col);
    quads->resize(n);
    circe::ColorPalette palette = circe::HEAT_MATLAB_PALETTE;
    ponos::RNGSampler sampler;
    ponos::HaltonSequence rng;
    for (size_t i = 0; i < n; i++) {
      auto color = palette((1.f * i) / n, 1.f);
      auto c = quads->instanceF(colid, i);
      c[0] = color.r;
      c[1] = color.g;
      c[2] = color.b;
      c[3] = color.a;
      c[3] = 0.4;
      auto m = quads->instanceF(tid, i);
      float t[16];
//      (ponos::scale(rng.randomFloat(), rng.randomFloat(), rng.randomFloat()) *
      (ponos::scale(1, 1, 1) *
          ponos::translate(ponos::vec3(sampler.sample(
              ponos::bbox3(ponos::point3(-5, 0, 0), ponos::point3(5, 5, 5))))))
          .matrix()
          .column_major(t);
      for (size_t k = 0; k < 16; k++)
        m[k] = t[k];
    }
  }
  //  app.scene.add(spheres.get());
  app.scene.add(quads.get());
  circe::gl::SceneObjectSPtr grid(new circe::gl::CartesianGrid(5));
  app.scene.add(grid.get());
  app.run();
  return 0;
}
