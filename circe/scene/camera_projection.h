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

#ifndef CIRCE_SCENE_CAMERA_PROJECTION_H
#define CIRCE_SCENE_CAMERA_PROJECTION_H

#include <hermes/geometry/transform.h>

namespace circe {

class CameraProjection {
public:
  CameraProjection() = default;
  virtual ~CameraProjection() = default;
  // updates projection transform after changes
  virtual void update() = 0;

  float ratio{1.f}; //!< film size ratio
  float near{0.01f}; //!< near depth clip plane
  float far{1000.f};  //!< far depth clip plane
  hermes::vec2 clip_size; //!< window size (in pixels)
  hermes::Transform transform; //!< projection transform
  hermes::Transform inv_transform;
  hermes::transform_options options{hermes::transform_options::left_handed}; //!< flags to choose handedness, etc.
};

class PerspectiveProjection : public CameraProjection {
public:
  PerspectiveProjection() = default;
  /// \param fov field of view angle (in degrees)
  /// \param options handedness, zero_to_one, flip_y, and other options
  explicit PerspectiveProjection(float fov,
                                 hermes::transform_options options = hermes::transform_options::left_handed)
      : fov(fov) {
    this->options = options;
  }
  void update() override {
    this->transform = hermes::Transform::perspective(fov,
                                                     this->ratio,
                                                     this->near,
                                                     this->far,
                                                     options);
  }

  float fov{45.f}; //!< field of view angle in degrees
};

class OrthographicProjection : public CameraProjection {
public:
  OrthographicProjection() {
    region_.lower.x = region_.lower.y = this->near = -1.f;
    region_.upper.x = region_.upper.y = this->far = 1.f;
  }
  OrthographicProjection(float left, float right, float bottom, float top,
                         hermes::transform_options options = hermes::transform_options::left_handed) {
    HERMES_UNUSED_VARIABLE(options)
    set(left, right, bottom, top);
  }
  /// \param z
  void zoom(float z) { region_ = hermes::Transform2::scale({z, z})(region_); }
  /// \param left
  /// \param right
  /// \param bottom
  /// \param top
  void set(float left, float right, float bottom, float top) {
    region_.lower.x = left;
    region_.lower.y = bottom;
    region_.upper.x = right;
    region_.upper.y = top;
    update();
  }
  void update() override {
    this->transform =
        hermes::Transform::ortho(region_.lower.x, region_.upper.x, region_.lower.y,
                                 region_.upper.y, this->near, this->far);
  }

private:
  hermes::bbox2 region_;
};

} // namespace circe

#endif // CIRCE_SCENE_CAMERA_PROJECTION_H
