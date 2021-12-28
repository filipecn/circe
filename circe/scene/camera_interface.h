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

#ifndef CIRCE_SCENE_CAMERA_H
#define CIRCE_SCENE_CAMERA_H

#include <hermes/geometry/transform.h>
#include <circe/scene/camera_projection.h>
#include <hermes/geometry/line.h>
#include <hermes/geometry/plane.h>
#include <hermes/geometry/frustum.h>

#include <utility>

namespace circe {

class CameraInterface {
public:
  /// \param p projection type
  CameraInterface() = default;
  virtual ~CameraInterface() = default;
  /// \param p normalized mouse position
  /// \return camera's ray in world space
  [[nodiscard]] virtual hermes::Ray3 pickRay(hermes::point2 p) const {
    hermes::point3 O = hermes::inverse(model * view)(
        hermes::inverse(projection->transform) * hermes::point3(p.x, p.y, 0.f));
    hermes::point3 D = hermes::inverse(model * view)(
        hermes::inverse(projection->transform) * hermes::point3(p.x, p.y, 1.f));
    return hermes::Ray3(O, D - O);
  }

  template<typename T, class... P>
  void setProjection(P &&...params) {
    projection = std::make_unique<T>(std::forward<P>(params)...);
  }
  /// \param w width in pixels
  /// \param h height in pixels
  virtual void resize(float w, float h) {
    projection->ratio = w / h;
    projection->clip_size = hermes::vec2(zoom, zoom);
    if (w < h)
      projection->clip_size.y = projection->clip_size.x / projection->ratio;
    else
      projection->clip_size.x = projection->clip_size.y * projection->ratio;
    projection->update();
    update();
  }
  /// \return MVP transform
  [[nodiscard]] virtual hermes::Transform getTransform() const {
    return projection->transform * view * model;
    return model * view * projection->transform;
  }
  /// \return camera projection const ptr
  [[nodiscard]] const CameraProjection *getCameraProjection() const {
    return projection.get();
  }
  /// \return projection transform
  [[nodiscard]] hermes::Transform getProjectionTransform() const {
    return projection->transform;
  }
  /// \return model transform
  [[nodiscard]] virtual hermes::Matrix3x3<real_t> getNormalMatrix() const { return normal; }
  [[nodiscard]] virtual hermes::Transform getModelTransform() const { return model; }
  [[nodiscard]] virtual hermes::Transform getViewTransform() const { return view; }
  /// \param p normalized mouse position
  /// \return line object in world space
  [[nodiscard]] virtual hermes::Line viewLineFromWindow(hermes::point2 p) const {
    hermes::point3 O = hermes::inverse(model * view)(
        hermes::inverse(projection->transform) * hermes::point3(p.x, p.y, 0.f));
    hermes::point3 D = hermes::inverse(model * view)(
        hermes::inverse(projection->transform) * hermes::point3(p.x, p.y, 1.f));
    return {O, D - O};
  }
  /// Applies **t** to camera transform (usually model transformation)
  /// \param t transform
  virtual void applyTransform(const hermes::Transform &t) {
    pos = target + t(pos - target) + t.getTranslate();
    target += t.getTranslate();
    up = t(up);
    // model = model * t;
    update();
  }
  /// \param p point in world space
  /// \return plane containing **p**, perpendicular to camera's view axis
  [[nodiscard]] virtual hermes::Plane viewPlane(hermes::point3 p) const {
    hermes::vec3 n = pos - p;
    if (fabs(n.length()) < 1e-8)
      n = hermes::vec3(0, 0, 0);
    else
      n = hermes::normalize(n);
    return {hermes::normal3(n), hermes::dot(n, hermes::vec3(p.x, p.y, p.z))};
  }
  /// \return up vector
  [[nodiscard]] virtual hermes::vec3 getUpVector() const { return up; };
  /// \return eye position
  [[nodiscard]] virtual hermes::point3 getPosition() const { return pos; };
  /// \return  target position
  [[nodiscard]] virtual hermes::point3 getTarget() const { return target; }
  [[nodiscard]] virtual hermes::vec3 getDirection() const { return target - pos; }
  [[nodiscard]] virtual hermes::vec3 getRight() const {
    return hermes::normalize(hermes::cross(up, target - pos));
  }
  /// \param p target position
  virtual void setTarget(hermes::point3 p) {
    target = p;
    update();
  }
  /// \param p eye position
  virtual void setPosition(hermes::point3 p) {
    pos = p;
    update();
  }
  /// \param z
  virtual void setZoom(float z) {
    zoom = z;
    update();
  }
  /// \param f
  virtual void setFar(float f) {
    projection->far = f;
    projection->update();
  }
  /// \param n
  virtual void setNear(float n) {
    projection->near = n;
    projection->update();
  }
  [[nodiscard]] virtual float getNear() const { return projection->near; }
  [[nodiscard]] virtual float getFar() const { return projection->far; }
  [[nodiscard]] virtual const hermes::Frustum &getFrustum() const { return frustum; }
  ///
  virtual void update() = 0;

protected:
  hermes::Transform view, inv_view;
  hermes::Transform model, inv_model;
  hermes::Frustum frustum;
  hermes::Matrix3x3<real_t> normal;
  hermes::vec3 up;
  hermes::point3 pos;
  hermes::point3 target;
  std::unique_ptr<CameraProjection> projection;
  float zoom{1.};
};

} // namespace circe

#endif // CIRCE_SCENE_CAMERA_H
