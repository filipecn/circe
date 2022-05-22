/// Copyright (c) 2022, FilipeCN.
///
/// The MIT License (MIT)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
///
///\file camera_control.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2022-03-04
///
///\brief

#include <circe/ui/camera_control.h>
#include <hermes/geometry/queries.h>

namespace circe {

/// Hit view plane at target distance
/// \param camera
/// \param ndc
/// \return
hermes::point3 hitViewPlane(CameraInterface &camera, const hermes::point2 &ndc) {
  hermes::Line l = camera.viewLineFromWindow(ndc);
  hermes::Plane vp = camera.viewPlane(camera.getTarget());
  hermes::point3 hp;
  hermes::GeometricQueries::intersect(vp, l, hp);
  return hp;
}

void CameraControlModeInterface::start(const CameraInterface &camera, hermes::point2 p) {
  HERMES_UNUSED_VARIABLE(camera);
  start_ = p;
  dragging_ = true;
}

void CameraControlModeInterface::stop(CameraInterface &camera, hermes::point2 p) {
  HERMES_UNUSED_VARIABLE(p);
  HERMES_UNUSED_VARIABLE(camera);
  dragging_ = false;
}

CameraControlScaleMode::CameraControlScaleMode() : CameraControlModeInterface() {}

CameraControlScaleMode::~CameraControlScaleMode() = default;

void CameraControlScaleMode::update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) {
  if (d == hermes::vec2())
    return;
  HERMES_UNUSED_VARIABLE(p);
  HERMES_UNUSED_VARIABLE(camera);
  float scale = (d.y < 0.f) ? 1.1f : 0.9f;
  // TODO what if the projection is different?!
  auto* proj = dynamic_cast<circe::OrthographicProjection*>(camera.getProjection());
  HERMES_CHECK_EXP(proj);
  proj->zoom(scale);
}

CameraControlPanMode::CameraControlPanMode() : CameraControlModeInterface() {}

CameraControlPanMode::~CameraControlPanMode() = default;

void CameraControlPanMode::update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) {
  HERMES_UNUSED_VARIABLE(d);
  if (!dragging_)
    return;
  hermes::point3 a = hitViewPlane(camera, start_);
  hermes::point3 b = hitViewPlane(camera, p);
  // it is -(b - a), because moving the mouse up should move the camera down
  // (so the image goes up)
  auto translation = a - b;
  camera.setTarget(camera.getTarget() + translation);
  camera.setPosition(camera.getPosition() + translation);
  start_ = p;
}

CameraControlZMode::CameraControlZMode() : CameraControlModeInterface() {}

CameraControlZMode::~CameraControlZMode() = default;

void CameraControlZMode::update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) {
  HERMES_UNUSED_VARIABLE(d);
  if (!dragging_)
    return;
  hermes::point3 a = hitViewPlane(camera, start_);
  hermes::point3 b = hitViewPlane(camera, p);
  hermes::vec3 t = hermes::normalize(camera.getTarget() - camera.getPosition());
  if (p.y - start_.y < 0.f)
    t *= -1.f;
  t *= hermes::distance(a, b);
  camera.setTarget(camera.getTarget() + t);
  camera.setPosition(camera.getPosition() + t);
  start_ = p;
}

CameraControlOrbitMode::CameraControlOrbitMode() : CameraControlModeInterface() {}

CameraControlOrbitMode::~CameraControlOrbitMode() = default;

void CameraControlOrbitMode::update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) {
  HERMES_UNUSED_VARIABLE(d);
  if (!dragging_ || p == start_)
    return;
  auto direction = p - start_;

  // if it is too close from the poles, we allow just horizontal rotation
  auto up_angle = hermes::dot((camera.getPosition() - camera.getTarget()).normalized(), camera.getUpVector());
  if (1.f - std::fabs(up_angle) < 1e-3 && up_angle * direction.y < 0)
    direction.y = 0;
  if (std::abs(direction.x) > std::abs(direction.y))
    direction.y = 0;
  if (std::abs(direction.x) < std::abs(direction.y))
    direction.x = 0;

  auto t = camera.getPosition() - camera.getTarget();
  auto u = camera.getUpVector();
  auto left = hermes::cross(t.normalized(), u).normalized();
  auto new_pos = camera.getPosition();

  // translate target to center
  new_pos -= (hermes::vec3) camera.getTarget();

  // the distance between coordinates -1 and 1 is equivalent to a full rotation
  // here we rotate camera position
  float angle = hermes::Constants::pi * direction.x;
  auto transform = hermes::Transform::rotate(angle, u);
  angle = hermes::Constants::pi * direction.y;
  transform = transform * hermes::Transform::rotate(-angle, left);
  new_pos = camera.getTarget() + (hermes::vec3) transform(new_pos);

  camera.setPosition(new_pos);

  start_ = p;
}

CameraControlZoomMode::CameraControlZoomMode() : CameraControlModeInterface() {}

CameraControlZoomMode::~CameraControlZoomMode() = default;

void CameraControlZoomMode::update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) {
  if (d == hermes::vec2())
    return;
  // get distance
  auto direction = camera.getTarget() - camera.getPosition();
  auto distance = direction.length();
  auto s = distance * 0.1f * (d.y < 0 ? -1.f : 1.f);
  camera.setPosition(camera.getPosition() + s * direction.normalized());
  start_ = p;
}

CameraControlFirstPersonMode::CameraControlFirstPersonMode() : CameraControlModeInterface() {}

CameraControlFirstPersonMode::~CameraControlFirstPersonMode() = default;

void CameraControlFirstPersonMode::update(CameraInterface &camera, hermes::point2 p, hermes::vec2 d) {
  HERMES_UNUSED_VARIABLE(d);
  if (!dragging_ || p == start_)
    return;
  auto direction = p - start_;

  // if it is too close from the poles, we allow just horizontal rotation
  auto up_angle = hermes::dot((camera.getPosition() - camera.getTarget()).normalized(), camera.getUpVector());
  if (1.f - std::fabs(up_angle) < 1e-3 && up_angle * direction.y < 0)
    direction.y = 0;
  if (std::abs(direction.x) > std::abs(direction.y))
    direction.y = 0;
  if (std::abs(direction.x) < std::abs(direction.y))
    direction.x = 0;

  auto t = camera.getPosition() - camera.getTarget();
  auto u = camera.getUpVector();
  auto left = hermes::cross(t.normalized(), u).normalized();
  auto new_target = camera.getTarget();

  // translate target to center
  new_target -= (hermes::vec3) camera.getPosition();

  // the distance between coordinates -1 and 1 is equivalent to a full rotation
  // here we rotate camera position
  float angle = hermes::Constants::pi * direction.x;
  auto transform = hermes::Transform::rotate(angle, u);
  angle = hermes::Constants::pi * direction.y;
  transform = transform * hermes::Transform::rotate(-angle, left);
  new_target = camera.getPosition() + (hermes::vec3) transform(new_target);

  camera.setTarget(new_target);

  start_ = p;
}

}
