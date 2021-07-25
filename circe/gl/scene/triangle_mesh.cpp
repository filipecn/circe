#include <circe/gl/scene/triangle_mesh.h>

#include <iostream>

namespace circe::gl {

TriangleMesh::TriangleMesh(const std::string &filename)
    : SceneMeshObject(filename) {}

TriangleMesh::TriangleMesh(const hermes::RawMesh *m) : SceneMeshObject(m) {}

void TriangleMesh::draw(const CameraInterface *camera,
                        hermes::Transform transform) {
  HERMES_UNUSED_VARIABLE(camera);
  HERMES_UNUSED_VARIABLE(transform);
}

} // namespace circe
