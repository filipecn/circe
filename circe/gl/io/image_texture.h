// Created by filipecn on 3/24/18.
/*
 * Copyright (c) 2018 FilipeCN
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

#ifndef CIRCE_IMAGE_TEXTURE_H
#define CIRCE_IMAGE_TEXTURE_H

#include <circe/colors/color_palette.h>
#include <circe/gl/io/texture.h>

namespace circe::gl {

class ImageTexture : public Texture {
public:
  /// \param w width number of texels
  /// \param h height number of texels
  /// \return Image texture filled by a checkboard pattern
  static ImageTexture checkBoard(size_t w, size_t h);
  /// \param w width number of texels
  /// \param h height number of texels
  ImageTexture(size_t w, size_t h);
  /// \param a texture attributes
  /// \param p texture parameters
  ImageTexture(const TextureAttributes &a, const TextureParameters &p);
  /// \param i coordinates in width axis
  /// \param j coordinates in height axis
  /// \param k [optional | default = 0] coordinates in depth axis
  /// \return color of texel (i,j,k)
  Color operator()(size_t i, size_t j, size_t k = 0);
  /// \param c color value
  /// \param i coordinates in width axis
  /// \param j coordinates in height axis
  /// \param k [optional | default = 0] coordinates in depth axis
  void setTexel(Color c, size_t i, size_t j, size_t k = 0);
  /// updates texture object with previously modified texels
  void update();
  friend std::ostream &operator<<(std::ostream &out, ImageTexture &it);

private:
  std::vector<unsigned char> data_;
};

} // circe namespace

#endif // CIRCE_IMAGE_TEXTURE_H
