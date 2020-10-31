#ifndef CIRCE_HELPERS_BVH_MODEL_H
#define CIRCE_HELPERS_BVH_MODEL_H

#include <ponos/ponos.h>

#include <circe/gl/helpers/geometry_drawers.h>
#include <circe/gl/scene/bvh.h>
#include <circe/gl/scene/scene_object.h>
#include <circe/gl/utils/open_gl.h>

namespace circe::gl {

/* bvh model
 * Draw BVH nodes
 */
class BVHModel : public SceneObject {
public:
  BVHModel(const BVH *_bvh) : bvh(_bvh) {}
  /* @inherit */
  void draw(const CameraInterface *camera,
            ponos::Transform transform) override {
    UNUSED_VARIABLE(camera);
    UNUSED_VARIABLE(transform);
    ponos::Transform inv = ponos::inverse(bvh->sceneMesh->transform);
    ponos::Ray3 r(ponos::point3(0, 0, 0), ponos::vec3(1, 1, 1));
    //    glBegin(GL_LINES);
    //    circe::glVertex(r.o);
    //    circe::glVertex(r.o + 1000.f * r.d);
    //    glEnd();
    recDraw(inv(r), bvh->root);
    return;
    static int k = 0;
    static int t = 0;
    t++;
    if (t > 1000) {
      t = 0;
      k = (k + 1) % bvh->nodes.size();
    }
    glLineWidth(1.f);
    for (size_t i = 0; i < bvh->nodes.size(); i++) {
      // if(i != k) continue;
      glColor4f(0, 0, 1, 0.4);
      if (bvh->nodes[i].nElements == 1)
        glColor4f(1, 0, 0, 0.8);
      draw_bbox(bvh->sceneMesh->transform(bvh->nodes[i].bounds));
    }
  }

  void recDraw(ponos::Ray3 r, BVH::BVHNode *n) const {
    if (!n)
      return;
    float a, b;
    if (ponos::bbox_ray_intersection(n->bounds, r, a, b)) {
      // if(!(n->children[0] || n->children[1])) {
      glColor4f(0, 0, 1, 1);
      draw_bbox(bvh->sceneMesh->transform(n->bounds));
      glColor4f(0, 0, 0, 0.3);
      //}
      // else{
      recDraw(r, n->children[0]);
      recDraw(r, n->children[1]);
      //}
    }
  }

  bool intersect(const ponos::Ray3 &r, float *t = nullptr) override {
    UNUSED_VARIABLE(t);
    ray = r;
    return false;
  }

private:
  const BVH *bvh;
  ponos::Ray3 ray;
};

} // namespace circe

#endif // CIRCE_HELPERS_CARTESIAN_GRID_H
