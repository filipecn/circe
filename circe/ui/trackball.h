#ifndef CIRCE_UI_TRACKBALL_H
#define CIRCE_UI_TRACKBALL_H

#include <hermes/geometry/transform.h>

namespace circe {

class Trackball {
public:
  Trackball() : radius_(5.f) {}

  [[nodiscard]] hermes::point3 center() const { return center_; }
  void setCenter(const hermes::point3 &center) { center_ = center; }
  [[nodiscard]] float radius() const { return radius_; }
  void setRadius(float r) { radius_ = r; }
  [[nodiscard]] hermes::Transform transform() const { return transform_ * partialTransform_; }
  hermes::Transform getPartialTransform() { return partialTransform_; }
  hermes::Transform getTransform() { return partialTransform_; }
  void setTransform(hermes::Transform t) { transform_ = t; }
  void setPartialTransform(hermes::Transform t) { partialTransform_ = t; }
  void accumulatePartialTransform(hermes::Transform t) {
    partialTransform_ = partialTransform_ * t;
  }
  void applyPartialTransform() {
    transform_ = transform_ * partialTransform_;
    partialTransform_.reset();
  }

private:
  hermes::point3 center_;
  hermes::Transform transform_;
  hermes::Transform partialTransform_;
  float radius_;
};

} // namespace circe

#endif // CIRCE_UI_TRACKBALL_H
