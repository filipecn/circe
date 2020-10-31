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

#ifndef CIRCE_UI_CAMERA_H
#define CIRCE_UI_CAMERA_H

#include <circe/scene/camera_interface.h>
#include <circe/ui/trackball_interface.h>

namespace circe {

/// Represents an user controllable camera through a trackball
class UserCamera : public CameraInterface {
public:
  UserCamera();
  /// process mouse button event
  /// \param action event type
  /// \param button button code
  /// \param p normalized mouse position
  void mouseButton(int action, int button, ponos::point2 p);
  /// process mouse move event
  /// \param p normalized mouse position
  void mouseMove(ponos::point2 p);
  /// process mouse wheel event
  /// \param p normalized mouse position
  /// \param d scroll vector
  virtual void mouseScroll(ponos::point2 p, ponos::vec2 d);
  TrackballInterface trackball;
};

class UserCamera2D : public UserCamera {
public:
  UserCamera2D();
  void mouseScroll(ponos::point2 p, ponos::vec2 d) override;
  void fit(const ponos::bbox2 &b, float delta = 1.f);
  void update() override;
};

class UserCamera3D : public UserCamera {
public:
  explicit UserCamera3D(bool left_handed = true);
  void setHandedness(bool left_handed);
  void setUp(const ponos::vec3 &u);
  void setFov(float f);
  void update() override;

private:
  bool left_handed_{true};
};

} // namespace circe

#endif // CIRCE_UI_CAMERA_H
