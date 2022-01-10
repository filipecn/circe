// Created by filipecn on 3/2/18.
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

#include <circe/gl/io/user_input.h>
#include <circe/ui/ui_camera.h>

#include <memory>

namespace circe {

UserCamera::UserCamera() = default;

void UserCamera::mouseButton(int action, int button, hermes::point2 p) {
  //  p.y = 1.f - p.y;
  if (action == PRESS)
    trackball.buttonPress(*this, button, p);
  else {
    trackball.buttonRelease(*this, button, p);
    trackball.tb.applyPartialTransform();
    applyTransform(trackball.tb.transform());
    trackball.tb.setTransform(hermes::Transform());
    trackball.tb.setCenter(target);
  }
}

void UserCamera::mouseMove(hermes::point2 p) {
  //  p.y = 1.f - p.y;
  if (trackball.isActive()) {
    trackball.mouseMove(*this, p);
    trackball.tb.applyPartialTransform();
    applyTransform(trackball.tb.transform());
    trackball.tb.setTransform(hermes::Transform());
    trackball.tb.setCenter(target);
  }
}

void UserCamera::mouseScroll(hermes::point2 p, hermes::vec2 d) {
  //  p.y = 1.f - p.y;
  trackball.mouseScroll(*this, p, d);
  trackball.tb.applyPartialTransform();
  applyTransform(trackball.tb.transform());
  trackball.tb.setTransform(hermes::Transform());
  trackball.tb.setCenter(target);
}

UserCamera2D::UserCamera2D() {
  this->projection = std::make_unique<OrthographicProjection>();
  this->pos = hermes::point3(0.f, 0.f, 1.f);
  this->target = hermes::point3(0.f, 0.f, 0.f);
  this->up = hermes::vec3(0.f, 1.f, 0.f);
  this->projection->near = -1000.f;
  this->projection->far = 1000.f;
}

void UserCamera2D::update() {
  auto view_vector = hermes::normalize(this->target - this->pos);
  if (hermes::Check::is_zero(hermes::cross(view_vector, this->up).length2()))
    this->up = {view_vector.y, view_vector.x, view_vector.z};
  this->view = hermes::Transform::lookAt(this->pos, this->target, this->up);
  inv_view = hermes::inverse(view);
  inv_model = hermes::inverse(model);
  normal = inverse((view * model).upperLeftMatrix());
  projection->inv_transform = inverse(projection->transform);
}

void UserCamera2D::fit(const hermes::bbox2 &b, float delta) {
  // setPos(hermes::vec2(b.center()));
  // setZoom((b.size(b.maxExtent()) / 2.f) * delta);
  setPosition(hermes::point3(b.center().x, b.center().y, 1.f));
  setTarget(hermes::point3(b.center().x, b.center().y, 0.f));
  dynamic_cast<OrthographicProjection *>(this->projection.get())
      ->zoom((b.size(b.maxExtent()) / 2.f) * delta);
  trackball.tb.setTransform(hermes::Transform());
  trackball.tb.setCenter(target);
  projection->update();
  update();
}
void UserCamera2D::mouseScroll(hermes::point2 p, hermes::vec2 d) {
  trackball.mouseScroll(*this, p, d);
  trackball.tb.applyPartialTransform();
  auto z = trackball.tb.transform()(hermes::point3(zoom, zoom, zoom));
  dynamic_cast<OrthographicProjection *>(this->projection.get())->zoom(z.x);
  trackball.tb.setTransform(hermes::Transform());
  trackball.tb.setCenter(target);
  projection->update();
}

UserCamera3D::UserCamera3D(hermes::transform_options options) {
  this->pos = hermes::point3(20.f, 0.f, 0.f);
  this->target = hermes::point3(0.f, 0.f, 0.f);
  this->up = hermes::vec3(0.f, 1.f, 0.f);
  this->zoom = 1.f;
  this->projection = std::make_unique<PerspectiveProjection>(45.f, options);
  this->projection->near = 0.1f;
  this->projection->far = 1000.f;
}

void UserCamera3D::setOptions(hermes::transform_options options) {
  projection->options = options;
  projection->update();
  update();
}

void UserCamera3D::setUp(const hermes::vec3 &u) {
  up = u;
  update();
}

void UserCamera3D::setFov(float f) {
  dynamic_cast<PerspectiveProjection *>(projection.get())->fov = f;
  projection->update();
  update();
}

void UserCamera3D::update() {
  view = hermes::Transform::lookAt(pos, target, up, projection->options);
  inv_view = hermes::inverse(view);
  inv_model = hermes::inverse(model);
  normal = inverse((model * view).upperLeftMatrix());
  frustum.set(model * view * projection->transform);
  trackball.tb.setRadius(hermes::distance(pos, target) / 3.f);
}

} // namespace circe
