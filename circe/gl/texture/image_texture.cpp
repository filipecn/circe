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

#include "image_texture.h"

namespace circe::gl {

ImageTexture::ImageTexture(size_t w, size_t h) {
  this->attributes_.target = GL_TEXTURE_2D;
  this->attributes_.width = w;
  this->attributes_.height = h;
  this->attributes_.type = GL_UNSIGNED_BYTE;
  this->attributes_.internal_format = GL_RGBA8;
  this->attributes_.format = GL_RGBA;
  data_.resize(w * h * 4, 0);
  glGenTextures(1, &this->texture_object_);
  glBindTexture(this->parameters_.target, this->texture_object_);
  this->parameters_.apply();
  update();
}

ImageTexture::ImageTexture(const TextureAttributes &a,
                           const TextureParameters &p)
    : Texture(a, p) {}

Color ImageTexture::operator()(size_t i, size_t j, size_t k) {
  size_t size = 4;
  size_t index = k * (this->attributes_.width * this->attributes_.height) +
                 j * this->attributes_.width + i;
  size_t baseIndex = index * size;
  float d = 1.f / 255.f;
  return {d * data_[baseIndex + 0], d * data_[baseIndex + 1],
          d * data_[baseIndex + 2], d * data_[baseIndex + 3]};
}

void ImageTexture::setTexel(Color c, size_t i, size_t j, size_t k) {
  size_t size = 4;
  size_t index = k * (this->attributes_.width * this->attributes_.height) +
                 j * this->attributes_.width + i;
  size_t baseIndex = index * size;
  data_[baseIndex + 0] = static_cast<unsigned char>(c.r * 255.f);
  data_[baseIndex + 1] = static_cast<unsigned char>(c.g * 255.f);
  data_[baseIndex + 2] = static_cast<unsigned char>(c.b * 255.f);
  data_[baseIndex + 3] = static_cast<unsigned char>(c.a * 255.f);
}

ImageTexture ImageTexture::checkBoard(size_t w, size_t h) {
  ImageTexture tex(w, h);
  size_t step = w / 10;
  for (size_t i = 0; i < w; i++)
    for (size_t j = 0; j < h; j++) {
      size_t row = j / step;
      size_t column = i / step;
      if (row % 2)
        tex.setTexel((column % 2) ? Color::Black() : Color::White(), i, j);
      else
        tex.setTexel((column % 2) ? Color::White() : Color::Black(), i, j);
    }
  CHECK_GL_ERRORS;
  tex.update();
  return tex;
}

void ImageTexture::update() {
  glBindTexture(this->parameters_.target, this->texture_object_);
  glTexImage2D(GL_TEXTURE_2D, 0, this->attributes_.internal_format,
               this->attributes_.width, this->attributes_.height, 0,
               this->attributes_.format, this->attributes_.type,
               &data_[0]);
  CHECK_GL_ERRORS;
  glBindTexture(attributes_.target, 0);
}

std::ostream &operator<<(std::ostream &out, ImageTexture &it) {
  {
    int width = it.attributes_.width;
    int height = it.attributes_.height;

    unsigned char *data = NULL;

    data = new unsigned char[(int)(width * height * 4)];

    for (int i(0); i < width; ++i) {
      for (int j(0); j < height; ++j) {
        data[j * width + i] = 0;
      }
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(it.attributes_.target, it.texture_object_);
    glGetTexImage(it.attributes_.target, 0, it.attributes_.format,
                  it.attributes_.type, data);

    CHECK_GL_ERRORS;

    out << width << " " << height << std::endl;

    for (int j(height - 1); j >= 0; --j) {
      for (int i(0); i < width; ++i) {
        out << "(";
        out << (int)data[(int)(j * width * 4 + i * 4 + 0)] << ",";
        out << (int)data[(int)(j * width * 4 + i * 4 + 1)] << ",";
        out << (int)data[(int)(j * width * 4 + i * 4 + 2)] << ",";
        out << (int)data[(int)(j * width * 4 + i * 4 + 3)] << ")";
      }
      out << std::endl;
    }
  }
  auto size = it.size();
  for (int i = size[1] - 1; i >= 0; i--) {
    for (size_t j = 0; j < size[0]; j++) {
      auto c = it(j, i, 0);
      out << "(" << c.r << "," << c.g << "," << c.b << "," << c.a << ")";
    }
    out << std::endl;
  }
  return out;
}

} // namespace circe