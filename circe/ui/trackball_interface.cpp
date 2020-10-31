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
#include <circe/ui/trackball_interface.h>

namespace circe {

TrackballInterface::~TrackballInterface() = default;

void TrackballInterface::draw() { modes_[curMode_]->draw(tb); }

void TrackballInterface::buttonRelease(CameraInterface &camera, int button,
                                       ponos::point2 p) {
  UNUSED_VARIABLE(button);
  if (curMode_ != Mode::NONE)
    modes_[curMode_]->stop(tb, camera, p);
}

void TrackballInterface::buttonPress(const CameraInterface &camera, int button,
                                     ponos::point2 p) {
  if (buttonMap_.find(button) == buttonMap_.end())
    return;
  curMode_ = buttonMap_[button];
  modes_[curMode_]->start(tb, camera, p);
}

void TrackballInterface::mouseMove(CameraInterface &camera, ponos::point2 p) {
  if (curMode_ != Mode::NONE)
    modes_[curMode_]->update(tb, camera, p, ponos::vec2());
}

void TrackballInterface::mouseScroll(CameraInterface &camera, ponos::point2 p,
                                     ponos::vec2 d) {
  if (buttonMap_.find(MOUSE_SCROLL) != buttonMap_.end()) {
    curMode_ = buttonMap_[MOUSE_SCROLL];
    modes_[curMode_]->update(tb, camera, p, d);
    curMode_ = Mode::NONE;
  }
}

void TrackballInterface::attachTrackMode(int button,
                                         TrackballInterface::Mode mode,
                                         TrackMode *tm) {
  buttonMap_[button] = mode;
  modes_[mode] = tm;
}

void TrackballInterface::createDefault2D(TrackballInterface &t) {
  t.attachTrackMode(MOUSE_SCROLL, Mode::SCALE, new ScaleMode());
  t.attachTrackMode(MOUSE_BUTTON_LEFT, Mode::PAN, new PanMode());
}

void TrackballInterface::createDefault3D(TrackballInterface &t) {
  t.attachTrackMode(MOUSE_SCROLL, Mode::SCALE, new ScaleMode());
  t.attachTrackMode(MOUSE_BUTTON_RIGHT, Mode::PAN, new PanMode());
  t.attachTrackMode(MOUSE_BUTTON_LEFT, Mode::ROTATE, new RotateMode());
  t.attachTrackMode(MOUSE_BUTTON_MIDDLE, Mode::Z, new ZMode());
}

bool TrackballInterface::isActive() const {
  auto m = modes_.find(curMode_);
  if (m != modes_.end())
    return m->second->isActive();
  return false;
}

} // namespace circe
