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

#ifndef CIRCE_UI_MODIFIER_CURSOR_H
#define CIRCE_UI_MODIFIER_CURSOR_H

#include <circe/colors/color_palette.h>
#include <circe/gl/scene/scene_object.h>
#include <functional>
#include <utility>

namespace circe::gl {

class ModifierCursor : public SceneObject {
public:
  ModifierCursor() {
    this->active = true;
    dragging = false;
  }
  virtual ~ModifierCursor() {}

  void mouse(CameraInterface &camera, hermes::point2 p) override {
    HERMES_UNUSED_VARIABLE(camera);
    hermes::point3 P = inverse(glGetMVPTransform())(p);
    last = position;
    position = hermes::point2(P.x, P.y);
    static bool first = true;
    if (first)
      last = position, first = false;
    mouseMove();
  }

  void button(CameraInterface &camera, hermes::point2 p, int button,
              int action) override {
    HERMES_UNUSED_VARIABLE(p);
    HERMES_UNUSED_VARIABLE(camera);
    if (action == GLFW_RELEASE) {
      dragging = false;
    } else {
      dragging = true;
      start = position;
    }
    mouseButton(button, action);
  }

  bool intersect(const hermes::Ray3 &r, float *t = nullptr) override {
    HERMES_UNUSED_VARIABLE(r);
    *t = 0.0000001;
    return true;
  }

  virtual void mouseMove() {}
  virtual void mouseButton(int b, int a) {
    HERMES_UNUSED_VARIABLE(a);
    HERMES_UNUSED_VARIABLE(b);
  }

  bool dragging;
  hermes::point2 position;
  hermes::point2 start;
  hermes::point2 last;
};

class CircleCursor : public ModifierCursor {
public:
  CircleCursor(const hermes::Circle &c,
               std::function<void(const CircleCursor &, hermes::vec2)> f,
               Color fc = Color::Black(), Color ac = Color::Red())
      : mouseCallback(std::move(f)), fillColor(fc), activeColor(ac), circle(c) {
    fillColor.a = 0.1;
    activeColor.a = 0.3;
  }
  ~CircleCursor() override = default;

  void draw(const CameraInterface *camera,
            hermes::Transform transform) override {
    HERMES_UNUSED_VARIABLE(camera);
    HERMES_UNUSED_VARIABLE(transform);
    hermes::Circle c = circle;
    if (this->dragging)
      glColor(activeColor);
    else
      glColor(fillColor);
    draw_circle(c);
  }

  void mouseMove() override {
    circle.c = this->position;
    if (mouseCallback)
      mouseCallback(*this, position - last);
  }

  void mouseButton(int b, int a) override {
    if (buttonCallback)
      buttonCallback(*this, b, a);
  }

  std::function<void(const CircleCursor &, hermes::vec2)> mouseCallback;
  std::function<void(const CircleCursor &, int b, int a)> buttonCallback;
  Color fillColor;
  Color activeColor;
  hermes::Circle circle;
};

} // namespace circe

#endif // CIRCE_UI_MODIFIER_CURSOR_H
