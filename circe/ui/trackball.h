#ifndef CIRCE_UI_TRACKBALL_H
#define CIRCE_UI_TRACKBALL_H

#include <ponos/ponos.h>

namespace circe {

class Trackball {
public:
  Trackball() : radius_(5.f) {}

  ponos::point3 center() const { return center_; }
  void setCenter(const ponos::point3 &center) { center_ = center; }
  float radius() const { return radius_; }
  void setRadius(float r) { radius_ = r; }
  ponos::Transform transform() const { return transform_ * partialTransform_; }
  ponos::Transform getPartialTransform() { return partialTransform_; }
  ponos::Transform getTransform() { return partialTransform_; }
  void setTransform(ponos::Transform t) { transform_ = t; }
  void setPartialTransform(ponos::Transform t) { partialTransform_ = t; }
  void accumulatePartialTransform(ponos::Transform t) {
    partialTransform_ = partialTransform_ * t;
  }
  void applyPartialTransform() {
    transform_ = transform_ * partialTransform_;
    partialTransform_.reset();
  }

private:
  ponos::point3 center_;
  ponos::Transform transform_;
  ponos::Transform partialTransform_;
  float radius_;
};

} // namespace circe

#endif // CIRCE_UI_TRACKBALL_H
