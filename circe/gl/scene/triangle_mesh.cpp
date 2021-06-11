#include <circe/gl/scene/triangle_mesh.h>

#include <iostream>

namespace circe::gl {

TriangleMesh::TriangleMesh(const std::string &filename)
    : SceneMeshObject(filename) {}

TriangleMesh::TriangleMesh(const ponos::RawMesh *m) : SceneMeshObject(m) {}

void TriangleMesh::draw(const CameraInterface *camera,
                        ponos::Transform transform) {
  PONOS_UNUSED_VARIABLE(camera);
  PONOS_UNUSED_VARIABLE(transform);
}

} // namespace circe
