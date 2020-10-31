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

#include <ponos/ponos.h>

namespace circe {

class CameraProjection {
public:
  CameraProjection() = default;
  virtual ~CameraProjection() = default;
  // updates projection transform after changes
  virtual void update() = 0;

  float ratio{1.f}; //!< film size ratio
  float znear{0.01f}; //!< z near clip plane
  float zfar{1000.f};  //!< z far clip plane
  ponos::vec2 clip_size; //!< window size (in pixels)
  ponos::Transform transform; //!< projection transform
};

class PerspectiveProjection : public CameraProjection {
public:
  PerspectiveProjection() = default;
  /// \param fov field of view angle (in degrees)
  /// \param left_handed  transform handedness
  explicit PerspectiveProjection(float fov,
                                 bool left_handed = true,
                                 bool zero_to_one = true, bool flip_y = true)
      : fov(fov), left_handed(left_handed), zero_to_one(zero_to_one), flip_y(flip_y) {}
  void update() override {
    if (left_handed)
      this->transform =
          ponos::perspective(fov, this->ratio, this->znear, this->zfar);
    else {
      this->transform = ponos::Transform::perspectiveRH(fov,
                                                        this->ratio,
                                                        this->znear,
                                                        this->zfar,
                                                        zero_to_one);
      if (flip_y) {
        auto m = this->transform.matrix();
        m[1][1] *= -1;
        this->transform = ponos::Transform(m);
      }
    }
  }

  float fov{45.f}; //!< field of view angle in degrees
  bool left_handed{true};
  bool zero_to_one{true};
  bool flip_y{true};
};

class OrthographicProjection : public CameraProjection {
public:
  OrthographicProjection() {
    region_.lower.x = region_.lower.y = this->znear = -1.f;
    region_.upper.x = region_.upper.y = this->zfar = 1.f;
  }
  OrthographicProjection(float left, float right, float bottom, float top) {
    set(left, right, bottom, top);
  }
  /// \param z
  void zoom(float z) { region_ = ponos::scale(z, z)(region_); }
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
        ponos::ortho(region_.lower.x, region_.upper.x, region_.lower.y,
                     region_.upper.y, this->znear, this->zfar);
  }

private:
  ponos::bbox2 region_;
};

} // namespace circe

#endif // CIRCE_SCENE_CAMERA_PROJECTION_H
