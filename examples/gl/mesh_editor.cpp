//
// Created by filipecn on 15/02/2021.
//

#include <circe/circe.h>

class MeshIntersector {
public:
  struct Triangle {
    [[nodiscard]] const ponos::bbox3 &bounds() const { return box; }
    ponos::bbox3 box{};
    u64 id{};
  };
  MeshIntersector() = default;
  void load(const circe::gl::SceneModel *mesh) {
    this->mesh = mesh;
    triangles.clear();
    assert(mesh->indexBuffer().element_type == GL_TRIANGLES);
    const auto &indices = mesh->model().indices();
    // generate data
    auto positions = mesh->model().data().field<ponos::point3>("position");
    for (u64 i = 0; i < mesh->elementCount(); ++i)
      triangles.push_back({ponos::make_union(ponos::bbox3(positions[indices[i * 3 + 0]],
                                                          positions[indices[i * 3 + 1]]),
                                             positions[indices[i * 3 + 2]]), i});
    bvh = std::make_shared<ponos::BVH<Triangle>>(triangles);
  }
  void intersect(const ponos::ray3 &r) {
    if (!bvh)
      return;
    auto positions = mesh->model().data().field<ponos::point3>("position");
    const auto &indices = mesh->model().indices();
    auto t = bvh->intersect(r, [&](const Triangle &trig) -> std::optional<f32> {
      auto a = positions[indices[trig.id * 3 + 0]];
      auto b = positions[indices[trig.id * 3 + 1]];
      auto c = positions[indices[trig.id * 3 + 2]];
      return ponos::GeometricPredicates::intersect(a, b, c, r);
    });
    if (t.has_value()) {
      last_selected_id = selected_id;
      selected_id = t.value().id;
    }
  }

  u64 last_selected_id{};
  u64 selected_id{};
  const circe::gl::SceneModel *mesh{nullptr};
  std::vector<Triangle> triangles;
  std::shared_ptr<ponos::BVH<Triangle>> bvh;
};

class MeshEditor : public circe::gl::BaseApp {
public:
  MeshEditor() : BaseApp(800, 800) {
    // setup object model
    ponos::Path assets_path(std::string(ASSETS_PATH));
    // setup meshes
    ponos::Path shaders_path(std::string(SHADERS_PATH));
    auto model = circe::io::readOBJ(assets_path + "suzanne.obj");
    wireframe_mesh = circe::Shapes::convert(model, circe::shape_options::wireframe);
    if (!wireframe_mesh.program.link(shaders_path, "hello"))
      spdlog::error("Failed to load model shader: " + wireframe_mesh.program.err);
    mesh = model;
    if (!mesh.program.link(shaders_path, "select"))
      spdlog::error("Failed to load model shader: " + mesh.program.err);
    intersector.load(&mesh);

    this->app_->mouseCallback = [&](f64 x, f64 y) {
      auto l = this->app_->getCamera()->viewLineFromWindow(this->app_->viewports[0].getMouseNPos());
      intersector.intersect({l.a, l.direction()});
      box = intersector.triangles[intersector.selected_id].bounds();
    };
    this->app_->scene.add(&box);

    nmesh = circe::Shapes::fromNMesh(ponos::NMesh::buildFrom(
        {
            {0.f, 0.f, 0.f},
            {1.f, 0.f, 0.f},
            {1.f, 1.f, 0.f},
            {0.f, 1.f, 0.f},
        }, {0, 1, 2, 3}, {4}
    ), circe::shape_options::wireframe);

    ponos::PMesh<3> pm;
    pm.buildFrom({
                     {0.f, 0.f, 0.f},
                     {1.f, 0.f, 0.f},
                     {0.5f, 1.f, 0.f},
                     {0.f, 1.f, 0.f},
                 }, {0, 1, 2}
    );
    pmesh = circe::Shapes::fromPMesh(pm, circe::shape_options::wireframe);

  }
  void render(circe::CameraInterface *camera) override {
    mesh.program.use();
    mesh.program.setUniform("view",
                            ponos::transpose(camera->getViewTransform().matrix()));
    mesh.program.setUniform("model", ponos::transpose(mesh.transform.matrix()));
    mesh.program.setUniform("projection",
                            ponos::transpose(camera->getProjectionTransform().matrix()));
    mesh.program.setUniform("selected", (int) intersector.selected_id);
    mesh.draw();
    wireframe_mesh.program.use();
    wireframe_mesh.program.setUniform("view",
                                      ponos::transpose(camera->getViewTransform().matrix()));
    wireframe_mesh.program.setUniform("model", ponos::transpose(mesh.transform.matrix()));
    wireframe_mesh.program.setUniform("projection",
                                      ponos::transpose(camera->getProjectionTransform().matrix()));
    wireframe_mesh.draw();
    nmesh.draw();
    pmesh.draw();
  }

  circe::gl::SceneModel nmesh, pmesh;
  circe::gl::SceneModel mesh;
  circe::gl::SceneModel wireframe_mesh;
  circe::gl::BBoxModel box;
  MeshIntersector intersector;
};

int main() {
  return MeshEditor().run();
}