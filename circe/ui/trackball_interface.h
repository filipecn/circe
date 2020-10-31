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

#ifndef CIRCE_UI_TRACKBALL_INTERFACE_H
#define CIRCE_UI_TRACKBALL_INTERFACE_H

#include <circe/scene/camera_interface.h>
#include <circe/ui/track_mode.h>
#include <circe/ui/trackball.h>

#include <ponos/ponos.h>
#include <vector>

namespace circe {

class TrackballInterface {
public:
  enum class Mode { ROTATE, Z, PAN, SCALE, NONE };

  TrackballInterface() { curMode_ = Mode::NONE; }

  virtual ~TrackballInterface();

  static void createDefault2D(TrackballInterface &t);
  static void createDefault3D(TrackballInterface &t);

  void draw();
  /// process mouse button release event
  /// \param camera
  /// \param button button code
  /// \param p normalized mouse position
  void buttonRelease(CameraInterface &camera, int button, ponos::point2 p);
  /// process mouse button press event
  /// \param camera
  /// \param button button code
  /// \param p normalized mouse position
  void buttonPress(const CameraInterface &camera, int button, ponos::point2 p);
  /// process mouse move event
  /// \param camera
  /// \param p normalized mouse position
  void mouseMove(CameraInterface &camera, ponos::point2 p);
  /// process mouse wheel event
  /// \param camera
  /// \param p normalized mouse position
  /// \param d scroll vector
  void mouseScroll(CameraInterface &camera, ponos::point2 p, ponos::vec2 d);
  /// Attaches a new mode to the interface
  /// \param button button to be mapped to mode
  /// \param mode attached mode
  /// \param tm track object
  void attachTrackMode(int button, Mode mode, TrackMode *tm);
  /// \return true if current mode is active
  [[nodiscard]] bool isActive() const;

  Trackball tb;

protected:
  Mode curMode_;
  /// map button -> mode
  std::map<int, Mode> buttonMap_;
  std::map<Mode, TrackMode *> modes_;
};

} // namespace circe

#endif // CIRCE_UI_TRACKBALL_INTERFACE_H
