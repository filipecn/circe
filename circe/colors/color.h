/// Copyright (c) 2017 FilipeCN
///
/// The MIT License (MIT)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// iM the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TOsurfaceRT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
///\file color.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2017-14-08
///
///\brief Color representations
///
///\ingroup colors
///\addtogroup colors
/// @{

#ifndef CIRCE_COLORS_COLOR_H
#define CIRCE_COLORS_COLOR_H

#include <hermes/geometry/vector.h>
#include <hermes/numeric/interpolation.h>

namespace circe {

// *********************************************************************************************************************
//                                                                                                              Color
// *********************************************************************************************************************
/// \brief RGBA color representation
class Color {
public:
  // *******************************************************************************************************************
  //                                                                                                   STATIC METHODS
  // *******************************************************************************************************************
  //                                                                                                       conversion
  /// \brief Constructs color from rgba [0,255] values
  /// \param r
  /// \param g
  /// \param b
  /// \param a
  /// \return
  static Color fromU32(u32 r, u32 g, u32 b, u32 a = 255) {
    return {r / 255.f, g / 255.f, b / 255.f, a / 255.f};
  }
  /// \brief Extracts RGB components from unsigned integer
  /// \param color
  /// \return
  static Color rbgFromU32(u32 color) {
    return fromU32((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
  }
  //                                                                                                           colors
  /// \brief Creates color transparent
  /// \return
  static Color Transparent() { return {0.f, 0.f, 0.f, 0.f}; }
  /// \brief Creates color black representation
  /// \param alpha opacity value
  /// \return
  static Color Black(float alpha = 1.f) { return {0.f, 0.f, 0.f, alpha}; }
  /// \brief Creates color white representation
  /// \param alpha opacity value
  /// \return
  static Color White(float alpha = 1.f) { return {1.f, 1.f, 1.f, alpha}; }
  /// \brief Creates color red representation
  /// \param alpha opacity value
  /// \return
  static Color Red(float alpha = 1.f) { return {1.f, 0.f, 0.f, alpha}; }
  /// \brief Creates color green representation
  /// \param alpha opacity value
  /// \return
  static Color Green(float alpha = 1.f) { return {0.f, 1.f, 0.f, alpha}; }
  /// \brief Creates color blue representation
  /// \param alpha opacity value
  /// \return
  static Color Blue(float alpha = 1.f) { return {0.f, 0.f, 1.f, alpha}; }
  /// \brief Creates color purple representation
  /// \param alpha opacity value
  /// \return
  static Color Purple(float alpha = 1.f) { return {1.f, 0.f, 1.f, alpha}; }
  /// \brief Creates color yellow representation
  /// \param alpha opacity value
  /// \return
  static Color Yellow(float alpha = 1.f) { return {1.f, 1.f, 0.f, alpha}; }
  /// \brief Creates color gray representation
  /// \param v common rgb value
  /// \param alpha opacity value
  /// \return
  static Color Gray(float v, float alpha = 1.f) { return {v, v, v, alpha}; }
  // *******************************************************************************************************************
  //                                                                                                     CONSTRUCTORS
  // *******************************************************************************************************************
  /// \brief Default constructor (color black)
  Color() {
    r = g = b = 0.f;
    a = 1.f;
  }
  /// \brief 3 [0,1] float component constructor
  /// \param v
  explicit Color(const hermes::vec3 &v) : r(v.x), g(v.y), b(v.z), a(1.f) {}
  /// \brief rgba [0,1] components constructor
  /// \param _r
  /// \param _g
  /// \param _b
  /// \param _a
  Color(float _r, float _g, float _b, float _a = 1.f)
      : r(_r), g(_g), b(_b), a(_a) {}
  // *******************************************************************************************************************
  //                                                                                                          METHODS
  // *******************************************************************************************************************
  /// \brief Creates a copy with a given opacity value
  /// \param alpha
  /// \return
  [[nodiscard]] Color withAlpha(float alpha) const { return {r, g, b, alpha}; }
  /// \brief Gets component array pointer
  /// \return
  [[nodiscard]] const float *asArray() const { return &r; }
  /// \brief Gets rgb components vector
  /// \return
  [[nodiscard]] hermes::vec3 rgb() const { return {r, g, b}; }
  // *******************************************************************************************************************
  //                                                                                                    PUBLIC FIELDS
  // *******************************************************************************************************************
  float r; //!< red component in [0,1] interval
  float g; //!< green component in [0,1] interval
  float b; //!< blue component in [0,1] interval
  float a; //!< opacity component in [0,1] interval
};

// *********************************************************************************************************************
//                                                                                                          FUNCTIONS
// *********************************************************************************************************************
/// \brief Linearly interpolates between two colors
/// \param t
/// \param a
/// \param b
/// \return
inline Color mix(float t, const Color &a, const Color &b) {
  return {hermes::interpolation::lerp(t, a.r, b.r), hermes::interpolation::lerp(t, a.g, b.g),
          hermes::interpolation::lerp(t, a.b, b.b)};
}

// *********************************************************************************************************************
//                                                                                                                 IO
// *********************************************************************************************************************
/// \brief Color support for `std::ostream` << operator
/// \param os
/// \param c
/// \return
inline std::ostream &operator<<(std::ostream &os, const Color c) {
  os << "Color [" << c.r << ", ";
  os << c.g << ", ";
  os << c.b << ", ";
  os << c.a << "]";
  return os;
}
} // namespace circe

#endif // CIRCE_COLORS_COLOR_H

/// @}
