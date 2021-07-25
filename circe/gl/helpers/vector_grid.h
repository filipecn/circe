#ifndef CIRCE_HELPERS_VECTOR_GRID_H
#define CIRCE_HELPERS_VECTOR_GRID_H

#include <circe/gl/scene/scene_object.h>
#include <circe/gl/utils/open_gl.h>

namespace circe::gl {

/* cartesian grid
 * Represents the main planes of a cartesian grid.
 */
class VectorGrid : public SceneObject {
public:
  VectorGrid(hermes::CGridInterface<hermes::vec3> &g) : grid(g) {}
  /* @inherit */
  void draw(const CameraInterface *camera,
            hermes::Transform transform) override {
    HERMES_UNUSED_VARIABLE(transform);
    HERMES_UNUSED_VARIABLE(camera);
    /*glColor4f(0, 0, 0, 0.7);
    glLineWidth(3.0);
    glBegin(GL_LINES);
    hermes::ivec3 ijk;
    FOR_INDICES0_3D(grid.dimensions, ijk) {
      hermes::point3 p = grid.toWorld(hermes::point3(ijk[0], ijk[1], ijk[2]));
      glVertex(p);
      glVertex(p + grid(ijk));
    }
    glEnd();
    glColor4f(0, 0, 0, 1);
    glPointSize(1);
    glBegin(GL_POINTS);
    FOR_INDICES0_3D(grid.dimensions, ijk) {
      hermes::point3 p = grid.toWorld(hermes::point3(ijk[0], ijk[1], ijk[2]));
      glVertex(p);
    }
    glEnd();
    glColor4f(0, 0, 0, 0.1);
    glLineWidth(1.0);
    glBegin(GL_LINES);
    hermes::ivec2 ij;
    FOR_INDICES0_E2D(grid.dimensions.xy(0, 1), ij) {
      glVertex(grid.toWorld(hermes::point3(ij[0] - 0.5f, ij[1] - 0.5f, -0.5f)));
      glVertex(grid.toWorld(hermes::point3(ij[0] - 0.5f, ij[1] - 0.5f,
                                          grid.dimensions[2] - 1 + 0.5f)));
    }
    FOR_INDICES0_E2D(grid.dimensions.xy(0, 2), ij) {
      glVertex(grid.toWorld(hermes::point3(ij[0] - 0.5f, -0.5f, ij[1] - 0.5f)));
      glVertex(grid.toWorld(hermes::point3(
          ij[0] - 0.5f, grid.dimensions[1] - 1 + 0.5f, ij[1] - 0.5f)));
    }
    FOR_INDICES0_E2D(grid.dimensions.xy(1, 2), ij) {
      glVertex(grid.toWorld(hermes::point3(-0.5f, ij[0] - 0.5f, ij[1] - 0.5f)));
      glVertex(grid.toWorld(hermes::point3(grid.dimensions[0] - 1 + 0.5f,
                                          ij[0] - 0.5f, ij[1] - 0.5f)));
    }
    glEnd();*/
  }

  hermes::CGridInterface<hermes::vec3> &grid;
};

} // namespace circe

#endif
