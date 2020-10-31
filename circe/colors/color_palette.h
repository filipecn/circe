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
#ifndef CIRCE_COLORS_COLOR_PALETTE_H
#define CIRCE_COLORS_COLOR_PALETTE_H

#include <circe/colors/color.h>
#include <ponos/numeric/numeric.h>

#include <initializer_list>
#include <vector>

namespace circe {

class ProceduralColorPalette {
public:
  ProceduralColorPalette(const ponos::vec3 &A, const ponos::vec3 &B,
                         const ponos::vec3 &C, const ponos::vec3 &D)
      : a(A), b(B), c(C), d(D) {}

  Color operator()(float t) {
    return Color(a + b * cos((c * t + d) * ponos::Constants::two_pi));
  }
  ponos::vec3 a, b, c, d;
};

// ProceduralColorPalette palette(
//    ponos::vec3(0.5, 0.5, 0.5), ponos::vec3(0.5, 0.5, 0.5),
//    ponos::vec3(1.0, 1.0, 1.0), ponos::vec3(0.00, 0.33, 0.67));

class ColorPalette {
public:
  ColorPalette() : a(1.f) {}
  explicit ColorPalette(std::initializer_list<int> c) : ColorPalette() {
    int n = c.size() / 3;
    auto it = c.begin();
    for (int i = 0; i < n; i++) {
      float r = static_cast<float>(*it++) / 255.f;
      float g = static_cast<float>(*it++) / 255.f;
      float b = static_cast<float>(*it++) / 255.f;
      colors.emplace_back(r, g, b);
    }
  }
  explicit ColorPalette(std::initializer_list<double> c) : ColorPalette() {
    int n = c.size() / 3;
    auto it = c.begin();
    for (int i = 0; i < n; i++) {
      float r = *it++;
      float g = *it++;
      float b = *it++;
      colors.emplace_back(r, g, b);
    }
  }
  Color operator()(float t, float alpha = -1) const {
    float ind = ponos::lerp(t, 0.f, static_cast<float>(colors.size()));
    float r = ind - ponos::floor2Int(ind);
    Color c;
    if (ponos::ceil2Int(ind) >= static_cast<int>(colors.size()))
      c = colors[colors.size() - 1];
    else
      c = mix(r, colors[ponos::floor2Int(ind)], colors[ponos::ceil2Int(ind)]);
    if (alpha >= 0.0)
      c.a = alpha;
    else
      c.a = a;
    return c;
  }
  float a;
  std::vector<Color> colors;
};

#define HEAT_MATLAB_PALETTE                                                    \
  ColorPalette(                                                                \
      {0.64706f,  0.f,       0.14902f, 0.67059f,   0.015686f, 0.14902f,        \
       0.69412f,  0.035294f, 0.14902f, 0.71373f,   0.05098f,  0.14902f,        \
       0.73725f,  0.070588f, 0.14902f, 0.75686f,   0.086275f, 0.14902f,        \
       0.77647f,  0.10588f,  0.14902f, 0.79608f,   0.12549f,  0.15294f,        \
       0.81176f,  0.1451f,   0.15294f, 0.82745f,   0.16863f,  0.15294f,        \
       0.84314f,  0.18824f,  0.15294f, 0.85882f,   0.20784f,  0.15686f,        \
       0.87059f,  0.23137f,  0.16078f, 0.88627f,   0.2549f,   0.17255f,        \
       0.89804f,  0.27843f,  0.18039f, 0.9098f,    0.30196f,  0.19608f,        \
       0.92157f,  0.32941f,  0.20784f, 0.93333f,   0.35294f,  0.22353f,        \
       0.94118f,  0.37647f,  0.23529f, 0.94902f,   0.40392f,  0.25098f,        \
       0.95686f,  0.42745f,  0.26275f, 0.96078f,   0.45098f,  0.27451f,        \
       0.96863f,  0.47843f,  0.28627f, 0.97255f,   0.50588f,  0.29804f,        \
       0.97647f,  0.53333f,  0.30588f, 0.98039f,   0.55686f,  0.31765f,        \
       0.98431f,  0.58431f,  0.32941f, 0.98824f,   0.61176f,  0.34118f,        \
       0.98824f,  0.63529f,  0.35294f, 0.99216f,   0.65882f,  0.36863f,        \
       0.99216f,  0.68235f,  0.38039f, 0.99216f,   0.70588f,  0.39608f,        \
       0.99216f,  0.72549f,  0.40784f, 0.99216f,   0.74902f,  0.42353f,        \
       0.99608f,  0.76863f,  0.43922f, 0.99608f,   0.78824f,  0.45882f,        \
       0.99608f,  0.80784f,  0.47451f, 0.99608f,   0.82745f,  0.4902f,         \
       0.99608f,  0.84706f,  0.5098f,  0.99608f,   0.86275f,  0.52549f,        \
       0.99608f,  0.87843f,  0.5451f,  0.99608f,   0.89412f,  0.56471f,        \
       0.99608f,  0.9098f,   0.58824f, 0.99608f,   0.92549f,  0.61569f,        \
       0.99608f,  0.94118f,  0.64314f, 1.f,        0.95686f,  0.67059f,        \
       1.f,       0.97255f,  0.69412f, 1.f,        0.98431f,  0.71765f,        \
       1.f,       0.99216f,  0.73333f, 1.f,        0.99608f,  0.7451f,         \
       1.f,       1.f,       0.74902f, 0.99608f,   1.f,       0.7451f,         \
       0.98824f,  0.99608f,  0.72941f, 0.97255f,   0.98824f,  0.70588f,        \
       0.95686f,  0.98039f,  0.67843f, 0.93725f,   0.97255f,  0.65098f,        \
       0.91373f,  0.96471f,  0.61961f, 0.8902f,    0.95294f,  0.59216f,        \
       0.87059f,  0.9451f,   0.56471f, 0.85098f,   0.93725f,  0.5451f,         \
       0.83137f,  0.92941f,  0.52941f, 0.81569f,   0.92157f,  0.51373f,        \
       0.79608f,  0.91373f,  0.49412f, 0.77647f,   0.90588f,  0.47843f,        \
       0.75686f,  0.89804f,  0.46667f, 0.73725f,   0.88627f,  0.45098f,        \
       0.71373f,  0.87843f,  0.43922f, 0.69412f,   0.87059f,  0.43137f,        \
       0.67451f,  0.85882f,  0.41961f, 0.65098f,   0.85098f,  0.41569f,        \
       0.62745f,  0.84314f,  0.41176f, 0.60392f,   0.83137f,  0.40784f,        \
       0.58039f,  0.81961f,  0.40392f, 0.55686f,   0.81176f,  0.40392f,        \
       0.53333f,  0.8f,      0.4f,     0.50588f,   0.78824f,  0.4f,            \
       0.47843f,  0.77647f,  0.39608f, 0.4549f,    0.76471f,  0.39608f,        \
       0.42745f,  0.75294f,  0.39216f, 0.4f,       0.74118f,  0.38824f,        \
       0.37255f,  0.72941f,  0.38431f, 0.33725f,   0.71373f,  0.37647f,        \
       0.30588f,  0.70196f,  0.37255f, 0.27059f,   0.68627f,  0.36471f,        \
       0.23529f,  0.67451f,  0.35686f, 0.20392f,   0.65882f,  0.34902f,        \
       0.17255f,  0.64314f,  0.34118f, 0.1451f,    0.62745f,  0.32941f,        \
       0.12157f,  0.61176f,  0.32157f, 0.10196f,   0.59608f,  0.31373f,        \
       0.086275f, 0.58039f,  0.30588f, 0.070588f,  0.56078f,  0.29804f,        \
       0.058824f, 0.5451f,   0.28627f, 0.043137f,  0.52549f,  0.27843f,        \
       0.031373f, 0.50588f,  0.26667f, 0.023529f,  0.4902f,   0.25882f,        \
       0.011765f, 0.47059f,  0.24706f, 0.0078431f, 0.45098f,  0.23529f,        \
       0.f,       0.42745f,  0.22745f, 0.f,        0.40784f,  0.21569f})

#define HEAT_1_COLOR_PALETTE                                                   \
  ColorPalette(                                                                \
      {246, 170, 111, 238, 132, 110, 215, 99, 105, 162, 85, 94, 70, 83, 90})

#define HEAT_COLOR_PALETTE                                                     \
  ColorPalette(                                                                \
      {255, 171, 130, 255, 106, 79, 255, 71, 55, 255, 47, 47, 255, 0, 0})

#define HEAT_GREEN_COLOR_PALETTE                                               \
  ColorPalette({202, 3, 0, 255, 101, 25, 202, 206, 23, 56, 140, 4, 4, 115, 49})

} // namespace circe

#endif // CIRCE_COLORS_COLOR_PALETTE_H
