/*
 * Copyright (c) 2017 FilipeCN
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef CIRCE_UI_TRACK_MODE_H
#define CIRCE_UI_TRACK_MODE_H

#include <circe/scene/camera_interface.h>
#include <circe/ui/trackball.h>

#include <hermes/geometry/sphere.h>
#include <hermes/geometry/plane.h>
#include <hermes/geometry/line.h>
#include <hermes/geometry/queries.h>

#include <iostream>

namespace circe {

/// Defines the behavior of a trackball been manipulated.
class TrackMode {
public:
  TrackMode() : dragging_(false) {}
  virtual ~TrackMode() = default;
  /// Renders trackball mode helpers
  /// \param tb trackball reference
  virtual void draw(const Trackball &tb) { HERMES_UNUSED_VARIABLE(tb); }
  /// Starts the manipulation (usually triggered by a button press)
  /// \param tb trackball reference
  /// \param camera active viewport camera
  /// \param p mouse position in normalized window position (NPos)
  virtual void start(Trackball &tb, const CameraInterface &camera,
                     hermes::point2 p) {
    HERMES_UNUSED_VARIABLE(camera);
    HERMES_UNUSED_VARIABLE(tb);
    start_ = p;
    dragging_ = true;
  }
  /// Updates track mode state (usually after a mouse moviment or scroll)
  /// \param tb trackball reference
  /// \param camera active viewport camera
  /// \param p mouse position in normalized window position (NPos)
  /// \param d scroll delta
  virtual void update(Trackball &tb, CameraInterface &camera, hermes::point2 p,
                      hermes::vec2 d) = 0;
  /// Stops the manipulation (usually after a button release)
  /// \param tb trackball reference
  /// \param camera active viewport camera
  /// \param p mouse position in normalized window position (NPos)
  virtual void stop(Trackball &tb, CameraInterface &camera, hermes::point2 p) {
    HERMES_UNUSED_VARIABLE(p);
    HERMES_UNUSED_VARIABLE(camera);
    dragging_ = false;
    ////    camera.applyTransform(tb.transform * partialTransform_);
    ////    tb.transform = hermes::Transform();
    tb.applyPartialTransform();
  }
  /// \return true if active
  [[nodiscard]] bool isActive() const { return dragging_; }

protected:
  /// Casts a ray from mouse's position to view plane
  /// \param tb trackball reference
  /// \param camera active viewport camera
  /// \param p mouse position in normalized window position (NPos)
  /// \return hit position
  hermes::point3 hitViewPlane(Trackball &tb, CameraInterface &camera,
                              hermes::point2 p) {
    hermes::Line l = camera.viewLineFromWindow(p);
    hermes::Plane vp = camera.viewPlane(tb.center());
    hermes::point3 hp;
    hermes::GeometricQueries::intersect(vp, l, hp);
    return hp;
  }

  bool dragging_;
  hermes::point2 start_;
};

/// Applies a scale
class ScaleMode : public TrackMode {
public:
  ScaleMode() : TrackMode() {}
  ~ScaleMode() override = default;
  void update(Trackball &tb, CameraInterface &camera, hermes::point2 p,
              hermes::vec2 d) override {
    if (d == hermes::vec2())
      return;
    HERMES_UNUSED_VARIABLE(p);
    HERMES_UNUSED_VARIABLE(camera);
    HERMES_UNUSED_VARIABLE(tb);
    float scale = (d.y < 0.f) ? 1.1f : 0.9f;
    tb.accumulatePartialTransform(hermes::Transform::scale(scale, scale, scale));
  }
};

/// Applies a translation
class PanMode : public TrackMode {
public:
  PanMode() : TrackMode() {}
  ~PanMode() override = default;

  void draw(const Trackball &tb) override { HERMES_UNUSED_VARIABLE(tb); }

  void update(Trackball &tb, CameraInterface &camera, hermes::point2 p,
              hermes::vec2 d) override {
    HERMES_UNUSED_VARIABLE(d);
    if (!dragging_)
      return;
    hermes::point3 a = hitViewPlane(tb, camera, start_);
    hermes::point3 b = hitViewPlane(tb, camera, p);
    // it is -(b - a), because moving the mouse up should move the camera down
    // (so the image goes up)
    tb.accumulatePartialTransform(hermes::Transform::translate(a - b));
    start_ = p;
  }
};

/// Applies a translation in camera's z axis
class ZMode : public TrackMode {
public:
  ZMode() : TrackMode() {}
  ~ZMode() override = default;

  void update(Trackball &tb, CameraInterface &camera, hermes::point2 p,
              hermes::vec2 d) override {
    HERMES_UNUSED_VARIABLE(d);
    if (!dragging_)
      return;
    hermes::point3 a = hitViewPlane(tb, camera, start_);
    hermes::point3 b = hitViewPlane(tb, camera, p);
    hermes::vec3 dir =
        hermes::normalize(camera.getTarget() - camera.getPosition());
    if (p.y - start_.y < 0.f)
      dir *= -1.f;
    tb.accumulatePartialTransform(
        hermes::Transform::translate(dir * hermes::distance(a, b)));
    start_ = p;
  }
};

/// Applies a rotation
class RotateMode : public TrackMode {
public:
  RotateMode() : TrackMode() {}
  ~RotateMode() override = default;
  void draw(const Trackball &tb) override {
    HERMES_UNUSED_VARIABLE(tb);
//    hermes::Sphere s(tb.center(), tb.radius() * 2.f);
//    glColor4f(0, 0, 0, 0.5);
//    draw_sphere(s);
  }
  void update(Trackball &tb, CameraInterface &camera, hermes::point2 p,
              hermes::vec2 d) override {
    HERMES_UNUSED_VARIABLE(d);
    if (!dragging_ || p == start_)
      return;
    hermes::point3 a = hitSpherePlane(tb, camera, start_);
    hermes::point3 b = hitSpherePlane(tb, camera, p);
    hermes::vec3 axis =
        hermes::normalize(hermes::cross((a - tb.center()), (b - tb.center())));
    float phi = hermes::distance(a, b) / tb.radius();
    tb.accumulatePartialTransform(
        hermes::Transform::rotate(-hermes::Trigonometry::radians2degrees(phi) * 0.7f, axis));
    start_ = p;
  }

private:
  hermes::point3 hitSpherePlane(Trackball &tb, CameraInterface &camera,
                                hermes::point2 p) {
    hermes::Line l = camera.viewLineFromWindow(p);
    hermes::Plane vp = camera.viewPlane(tb.center());

    hermes::Sphere s(tb.center(), tb.radius());

    hermes::point3 hp;
    hermes::GeometricQueries::intersect(vp, l, hp);

    hermes::point3 hs, hs1, hs2;
    bool resSp = hermes::GeometricQueries::intersect(s, l, hs1, hs2);
    if (resSp) {
      if (hermes::distance(camera.getPosition(), hs1) <
          hermes::distance(camera.getPosition(), hs2))
        hs = hs1;
      else
        hs = hs2;
      return hs;
    }
    hermes::point3 hh;
    bool resHp = hitHyper(tb, camera.getPosition(), vp, hp, hh);
    if ((!resSp && !resHp))
      return l.closestPoint(tb.center());
    if ((resSp && !resHp))
      return hs;
    if ((!resSp && resHp))
      return hh;
    float angleDeg = hermes::Trigonometry::radians2degrees(
        asin(hermes::dot(hermes::normalize(camera.getPosition() - tb.center()),
                         hermes::normalize(hs - tb.center()))));
    if (angleDeg < 45)
      return hs;
    else
      return hh;
  }
  bool hitHyper(Trackball tb, hermes::point3 viewpoint, hermes::Plane vp,
                hermes::point3 hitplane, hermes::point3 &hit) {
    float hitplaney = hermes::distance(tb.center(), hitplane);
    float viewpointx = hermes::distance(tb.center(), viewpoint);

    float a = hitplaney / viewpointx;
    float b = -hitplaney;
    float c = (tb.radius() * tb.radius()) / 2.f;
    float delta = b * b - 4 * a * c;
    float x1, xval, yval;

    if (delta > 0) {
      x1 = (-b - sqrtf(delta)) / (2.f * a);
      xval = x1;
      yval = c / xval;
    } else
      return false;
    hermes::vec3 dirRadial = hermes::normalize(hitplane - tb.center());
    hermes::vec3 dirView = hermes::normalize(hermes::vec3(vp.normal));
    hit = tb.center() + dirRadial * yval + dirView * xval;
    return true;
  }
};

} // namespace circe

#endif // CIRCE_UI_TRACK_MODE_H
