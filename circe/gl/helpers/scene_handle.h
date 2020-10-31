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

#ifndef CIRCE_HELPERS_SCENE_HANDLE_H
#define CIRCE_HELPERS_SCENE_HANDLE_H

#include <circe/colors/color_palette.h>
#include <circe/gl/scene/scene_object.h>
#include <functional>

namespace circe::gl {

template <typename T> class CircleHandle : public SceneObject {
public:
  CircleHandle() {
    fillColor = selectedColor = Color::Blue();
    fillColor.a = 0.1f;
    circle.c = ponos::point2();
    circle.r = 0.1f;
  }
  void draw() const {
    glColor(fillColor);
    if (this->selected)
      glColor(selectedColor);
    // draw_circle(circle, &this->transform);
  }

  bool intersect(const ponos::Ray3 &r, float *t = nullptr) override {
    return ponos::distance2(circle.c, ponos::point2(r.o.x, r.o.y)) <=
           ponos::SQR(circle.r);
  }

  ponos::Circle circle;
  Color fillColor;
  Color selectedColor;
  std::function<void(T *)> updateCallback;

private:
  T *data;
};

} // namespace circe

#endif // CIRCE_HELPERS_SCENE_HANDLE_H
