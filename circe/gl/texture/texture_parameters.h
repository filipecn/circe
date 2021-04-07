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

#ifndef CIRCE_IO_TEXTURE_ATTRIBUTES_H
#define CIRCE_IO_TEXTURE_ATTRIBUTES_H

#include <circe/gl/utils/open_gl.h>

#include <cstring>
#include <map>

namespace circe::gl {

/** \brief specify a texture image
 *
 *  glTexImage3D(GL_TEXTURE_3D, 0, attributes.internalFormat, attributes.width,
 *  attributes.height, attributes.depth, 0, attributes.format, attributes.type,
 *  attributes.data);
 *  glTexImage2D(GL_TEXTURE_2D, 0, attributes.internalFormat, attributes.width,
 *  attributes.height, 0, attributes.format, attributes.type, attributes.data);
 */
struct TextureAttributes {
  size_t width = 0; //!< width of the texture (in texels)
  size_t height =
      0; //!< height of the texture (in texels) or the number of layers
  size_t depth =
      0; //!< height of the texture (in texels) or the number of layers
  GLint internal_format =
      0; //!< the color components in the texture (ex: GL_RGBA8)
  GLenum format =
      0;           //!< format of pixel data (ex: GL_RGBA, GL_RED_INTEGER, ...)
  GLenum type = 0; //!< data type of pixel data (ex: GL_UNSIGNED_BYTE, GL_FLOAT)
  GLenum target = 0;      //!< target texture (ex: GL_TEXTURE_3D)
};

/** \brief set of texture parameters
 */
struct TextureParameters {
  /** \brief Constructor
   * \param t texture target
   * \param bc border color
   */
  explicit TextureParameters(GLuint t = GL_TEXTURE_2D, float *bc = nullptr) {
    target = t;
    parameters[GL_TEXTURE_WRAP_S] = GL_CLAMP_TO_EDGE;
    parameters[GL_TEXTURE_WRAP_T] = GL_CLAMP_TO_EDGE;
    parameters[GL_TEXTURE_MIN_FILTER] = GL_LINEAR;
    parameters[GL_TEXTURE_MAG_FILTER] = GL_LINEAR;
    parameters[GL_TEXTURE_BASE_LEVEL] = 0;
    parameters[GL_TEXTURE_MAX_LEVEL] = 0;

    if (target == GL_TEXTURE_3D)
      parameters[GL_TEXTURE_WRAP_R] = GL_CLAMP_TO_EDGE;

    borderColor = nullptr;
    if (bc) {
      borderColor = new float[4];
      std::memcpy(borderColor, bc, 4 * sizeof(float));
      parameters[GL_TEXTURE_WRAP_S] = GL_CLAMP_TO_BORDER;
      parameters[GL_TEXTURE_WRAP_T] = GL_CLAMP_TO_BORDER;
      if (target == GL_TEXTURE_3D)
        parameters[GL_TEXTURE_WRAP_R] = GL_CLAMP_TO_BORDER;
    }
  }
  /** \brief calls glTexParameteri
   */
  void apply() const {
    for (auto &parameter : parameters)
      glTexParameteri(target, parameter.first, parameter.second);
    if (borderColor)
      glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
  }
  /** \brief add/edit parameter
   * \param k key (name of parameter (OpenGL enum)
   * \returns reference to parameter's value
   */
  GLuint &operator[](const GLuint &k) { return parameters[k]; }

  float *borderColor; //!< color used in GL_CLAMP_TO_BORDER
  GLenum target;      //!< target texture (ex: GL_TEXTURE_3D)

private:
  std::map<GLuint, GLuint> parameters;
};

} // namespace circe

#endif // CIRCE_IO_TEXTURE_ATTRIBUTES_H
