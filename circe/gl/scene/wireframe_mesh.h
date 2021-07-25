#ifndef CIRCE_HELPERS_WIREFRAME_MESH_H
#define CIRCE_HELPERS_WIREFRAME_MESH_H

#include <circe/gl/scene/scene_object.h>

namespace circe::gl {

class WireframeMesh : public SceneMeshObject {
public:
  WireframeMesh(const std::string &filename);
  WireframeMesh(hermes::RawMesh *m, const hermes::Transform &t);
  virtual ~WireframeMesh() {}
  /* @inherit */
  void draw(const CameraInterface *camera, hermes::Transform transform) override;

protected:
  void setupIndexBuffer();
};

} // circe namespace

#endif // CIRCE_HELPERS_WIREFRAME_MESH_H
