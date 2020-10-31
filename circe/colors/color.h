/*
 * Copyright (c) 2017 FilipeCN
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * iM the Software without restriction, including without limitation the rights
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
#ifndef CIRCE_COLORS_COLOR_H
#define CIRCE_COLORS_COLOR_H

#include <ponos/ponos.h>

namespace circe {

class Color {
public:
  Color() {
    r = g = b = 0.f;
    a = 1.f;
  }
  explicit Color(const ponos::vec3 &v) : r(v.x), g(v.y), b(v.z), a(1.f) {}
  Color(float _r, float _g, float _b, float _a = 1.f)
      : r(_r), g(_g), b(_b), a(_a) {}
  Color withAlpha(float alpha) { return {r, g, b, alpha}; }
  [[nodiscard]] const float *asArray() const { return &r; }
  static Color Transparent() { return {0.f, 0.f, 0.f, 0.f}; }
  static Color Black(float alpha = 1.f) { return {0.f, 0.f, 0.f, alpha}; }
  static Color White(float alpha = 1.f) { return {1.f, 1.f, 1.f, alpha}; }
  static Color Red(float alpha = 1.f) { return {1.f, 0.f, 0.f, alpha}; }
  static Color Green(float alpha = 1.f) { return {0.f, 1.f, 0.f, alpha}; }
  static Color Blue(float alpha = 1.f) { return {0.f, 0.f, 1.f, alpha}; }
  static Color Purple(float alpha = 1.f) { return {1.f, 0.f, 1.f, alpha}; }
  static Color Yellow(float alpha = 1.f) { return {1.f, 1.f, 0.f, alpha}; }
  [[nodiscard]] ponos::vec3 rgb() const { return {r, g, b}; }
  float r, g, b, a;
};

inline Color mix(float t, const Color &a, const Color &b) {
  return {ponos::lerp(t, a.r, b.r), ponos::lerp(t, a.g, b.g),
          ponos::lerp(t, a.b, b.b)};
}

//#define COLOR_TRANSPARENT Color(0.f, 0.f, 0.f, 0.f)
//#define COLOR_BLACK Color(0.f, 0.f, 0.f, 1.f)
//#define COLOR_WHITE Color(1.f, 1.f, 1.f, 1.f)
//#define COLOR_RED Color(1.f, 0.f, 0.f, 1.f)
//#define COLOR_GREEN Color(0.f, 1.f, 0.f, 1.f)
//#define COLOR_BLUE Color(0.f, 0.f, 1.f, 1.f)
//#define COLOR_PURPLE Color(1.f, 0.f, 1.f, 1.f)

} // namespace circe

#endif // CIRCE_COLORS_COLOR_H
