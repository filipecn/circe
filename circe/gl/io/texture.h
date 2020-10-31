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

#ifndef CIRCE_IO_TEXTURE_H
#define CIRCE_IO_TEXTURE_H

#include <circe/gl/io/texture_parameters.h>

namespace circe::gl {

/// Holds an OpenGL texture object
/// Texture objects are created and deleted upon initialization and destruction,
/// respectively. So when using a copy constructor or operator, a full copy of
/// the texture, including texels, is made.
class Texture {
public:
  // ***********************************************************************
  //                          STATIC METHODS
  // ***********************************************************************
  ///
  /// \param path
  /// \return Texture object
  static Texture fromFile(const ponos::Path &path);
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  Texture();
  /// Parameter constructor
  /// \param a texture attributes
  /// \param p texture parameters
  /// \param data
  Texture(const TextureAttributes &a, const TextureParameters &p, const void *data = nullptr);
  Texture(const Texture &other);
  Texture(Texture &&other) noexcept;
  virtual ~Texture();
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  Texture &operator=(const Texture &other);
  Texture &operator=(Texture &&other) noexcept;
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  /// Binds texture
  /// \param t texture unit (ex: GL_TEXTURE0)
  virtual void bind(GLenum t) const;
  /// Binds image texture to an image unit for  the purpose of reading and
  /// writing it from shaders
  /// \param t texture unit  (ex: GL_TEXTURE0)
  virtual void bindImage(GLenum t) const;
  ///
  void generateMipmap() const;
  // ***********************************************************************
  //                           GETTERS
  // ***********************************************************************
  /// retrieve texture pixel data
  /// \return list of pixels by row major
  [[nodiscard]] std::vector<unsigned char> texels() const;
  /// \return texture resolution in texels
  [[nodiscard]] ponos::size3 size() const;
  /// \return opengl object associated to this texture
  [[nodiscard]] GLuint textureObjectId() const;
  /// \return
  [[nodiscard]] GLenum target() const;
  /// \return
  [[nodiscard]] const TextureAttributes &attributes() const;
  /// \return
  [[nodiscard]] const TextureParameters &parameters() const;
  // ***********************************************************************
  //                           SETTERS
  // ***********************************************************************
  /// Textures can be copied, only moved
  /// \param a texture attributes
  /// \param p texture parameters
  /// \param data
  virtual void set(const TextureAttributes &a, const TextureParameters &p);
  /// \param k
  /// \param value
  void setParameter(GLuint k, GLuint value);
  /// \param texels texture content
  void setTexels(const void *texels) const;
  /// \param new_size
  void resize(const ponos::size3 &new_size);
  /// \param new_size
  void resize(const ponos::size2 &new_size);
  /// \param internal_format
  void setInternalFormat(GLint internal_format);
  /// \param format
  void setFormat(GLint format);
  /// \param type
  void setType(GLenum type);
  /// \param target
  void setTarget(GLenum target);

  friend std::ostream &operator<<(std::ostream &out, Texture &pt);

protected:
  GLuint texture_object_{};
  mutable bool parameters_changed_{true};
  TextureAttributes attributes_;
  TextureParameters parameters_;
};

} // namespace circe

#endif // CIRCE_IO_TEXTURE_H
