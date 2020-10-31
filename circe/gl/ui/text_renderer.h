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

#ifndef CIRCE_UI_TEXT_RENDERER_H
#define CIRCE_UI_TEXT_RENDERER_H

#include <map>

#include <circe/gl/io/font_texture.h>
#include <circe/gl/io/graphics_display.h>
#include <circe/gl/io/texture.h>
#include <circe/gl/scene/quad.h>
#include <circe/gl/utils/open_gl.h>
#include <sstream>

namespace circe::gl {

/// Draws texts on the screen.
class TextRenderer {
public:
  explicit TextRenderer(const std::string& filename);
  /// \param s text size
  /// \param c text color
  /// \param id font id (from font manager)
  explicit TextRenderer(float scale = 1.f, Color c = Color::Black(),
                        size_t id = 0);
  /// \brief draws text on screen from a screen position
  /// \param s text
  /// \param x pixel position (screen coordinates)
  /// \param y pixel position (screen coordinates)
  /// \param scale
  /// \param c color
  void render(std::string s, GLfloat x, GLfloat y, GLfloat scale = 1.f,
              circe::Color c = Color::Black());
  /// \brief draws text on screen from a screen position
  /// \param s text
  /// \param p pixel position (in world coordinates)
  /// \param scale
  /// \param c color
  void render(std::string s, const ponos::point3 &p,
              const CameraInterface *camera, GLfloat scale, circe::Color c);
  /// \param c camera pointer
  void setCamera(const CameraInterface *c);
  /// \param p position (world coordinates)
  /// \return text renderer reference
  TextRenderer &at(const ponos::point3 &p);
  /// \param p position (screen coordinates)
  /// \return text renderer reference
  TextRenderer &at(const ponos::point2 &p);
  /// \param s scale
  /// \return text renderer reference
  TextRenderer &withScale(float s);
  /// \param c color
  /// \return text renderer reference
  TextRenderer &withColor(Color c);
  /// \param tr text renderer reference
  /// \return text renderer reference
  TextRenderer &operator<<(TextRenderer &tr);
  template <typename T> TextRenderer &operator<<(T t) {
    std::stringstream s;
    s << t;
    if (camera_ && usingCamera_)
      render(s.str(), position_, camera_,
             (usingDynamicScale_) ? dynamicScale_ : textSize,
             (usingDynamicColor_) ? dynamicColor_ : textColor);
    else
      render(s.str(), position_.x, position_.y,
             (usingDynamicScale_) ? dynamicScale_ : textSize,
             (usingDynamicColor_) ? dynamicColor_ : textColor);
    usingDynamicScale_ = usingDynamicColor_ = false;
    return *this;
  }
  size_t fontId = 0;    //!< font id (from font manager)
  float textSize = 1.f; //!< text scale
  Color textColor;      //!< text color
private:
  bool usingCamera_ = false;
  bool usingDynamicScale_ = false;
  bool usingDynamicColor_ = false;
  ponos::point3 position_;
  float dynamicScale_ = 1.f;
  Color dynamicColor_;
  const CameraInterface *camera_ = nullptr; //!< reference camera
  Quad quad_;
  FontAtlas atlas;
};

} // namespace circe

#endif // CIRCE_UI_TEXT_H
