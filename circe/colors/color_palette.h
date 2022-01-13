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
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TOsurfaceRT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#ifndef CIRCE_COLORS_COLOR_PALETTE_H
#define CIRCE_COLORS_COLOR_PALETTE_H

#include <circe/colors/color.h>
#include <hermes/numeric/numeric.h>

#include <initializer_list>
#include <vector>

namespace circe {

class ProceduralColorPalette {
public:
  ProceduralColorPalette(const hermes::vec3 &A, const hermes::vec3 &B,
                         const hermes::vec3 &C, const hermes::vec3 &D)
      : a(A), b(B), c(C), d(D) {}

  Color operator()(float t) const {
    return Color(a + b * cos((c * t + d) * hermes::Constants::two_pi));
  }
  hermes::vec3 a, b, c, d;
};

// ProceduralColorPalette palette(
//    hermes::vec3(0.5, 0.5, 0.5), hermes::vec3(0.5, 0.5, 0.5),
//    hermes::vec3(1.0, 1.0, 1.0), hermes::vec3(0.00, 0.33, 0.67));

class ColorPalette {
public:
  static ColorPalette MatlabHeatMap();
  static ColorPalette Batlow();

  ColorPalette() : a(1.f) {}
  explicit ColorPalette(const u32 *c, size_t n);
  explicit ColorPalette(const f32 *c, size_t rgb_count);
  ColorPalette(std::initializer_list<int> c);
  ColorPalette(std::initializer_list<double> c);
  ///
  /// \param t
  /// \param alpha
  /// \return
  inline Color operator()(float t, float alpha = -1) const {
    t = hermes::Numbers::clamp(t, 0.f, 1.f);
    float ind = hermes::interpolation::lerp(t, 0.f, static_cast<float>(colors.size()));
    float r = std::abs(hermes::Numbers::fract(ind));
    Color c;

    auto upper = hermes::Numbers::ceil2Int(ind);
    auto lower = hermes::Numbers::floor2Int(ind);

    if (upper >= static_cast<int>(colors.size()))
      c = colors[colors.size() - 1];
    else if (lower < 0)
      c = colors[0];
    else
      c = mix(r, colors[lower], colors[upper]);
    if (alpha >= 0.0)
      c.a = alpha;
    else
      c.a = a;
    return c;
  }
  float a{1.f};
  std::vector<Color> colors;
};

} // namespace circe

#endif // CIRCE_COLORS_COLOR_PALETTE_H
