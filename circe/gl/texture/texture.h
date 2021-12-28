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

#include <circe/gl/utils/open_gl.h>
#include <circe/texture/texture_options.h>
#include <hermes/common/file_system.h>

namespace circe::gl {

/// Holds an OpenGL texture object
/// \note Texture objects are created and deleted upon initialization and destruction,
/// respectively. So when using a copy constructor or operator, a full copy of
/// the texture, including texels, is made.
class Texture {
public:
/** \brief specify a texture image
 *
 *  glTexImage3D(GL_TEXTURE_3D, 0, attributes.internalFormat, attributes.width,
 *  attributes.height, attributes.depth, 0, attributes.format, attributes.type,
 *  attributes.data);
 *  glTexImage2D(GL_TEXTURE_2D, 0, attributes.internalFormat, attributes.width,
 *  attributes.height, 0, attributes.format, attributes.type, attributes.data);
 */
  struct Attributes {
    hermes::size3 size_in_texels;
    GLint internal_format{GL_RGBA8}; //!< the color components in the texture (ex: GL_RGBA8)
    GLenum format{GL_RGBA}; //!< format of pixel data (ex: GL_RGBA, GL_RED_INTEGER, ...)
    GLenum type{GL_UNSIGNED_BYTE}; //!< data type of pixel data (ex: GL_UNSIGNED_BYTE, GL_FLOAT)
    GLenum target{GL_TEXTURE_2D};      //!< target texture (ex: GL_TEXTURE_3D)
  };
  // ***********************************************************************
  //                          TEXTURE ATLAS
  // ***********************************************************************
  /// A Texture Atlas arranges sub-regions of the texture. It can be very
  /// useful to store animation sprites, shadow maps and other resources
  /// that are usually accessed in the same draw loop.
  class Atlas {
  public:
    struct Region {
      hermes::size2 offset;
      hermes::size2 size;
    };
    Atlas();
    ~Atlas();
    // ***********************************************************************
    //                              SIZE
    // ***********************************************************************
    [[nodiscard]] const hermes::size2 &size_in_texels() const;
    // ***********************************************************************
    //                              REGIONS
    // ***********************************************************************
    size_t push(const Region &region);
    const Region &operator[](size_t i) const;
    Region &operator[](size_t i);
    [[nodiscard]] const hermes::bbox2 &uv(size_t i) const;
  private:
    void updateUVs();
    std::vector<Region> regions_;
    std::vector<hermes::bbox2> uvs_;
    hermes::size2 size_in_texels_;
  };
  // ***********************************************************************
  //                          TEXTURE VIEW
  // ***********************************************************************
  class View {
  public:
    /// \note Initializes with GL_CLAMP_TO_EDGE wrap mode
    /// \note Initializes with GL_LINEAR filter mode
    /// \param target [= GL_TEXTURE_2D]
    explicit View(GLuint target = GL_TEXTURE_2D);
    /// \note Initializes with GL_CLAMP_TO_BORDER wrap mode
    /// \note Initializes with GL_LINEAR filter mode
    /// \param border_color
    /// \param target [= GL_TEXTURE_2D]
    explicit View(circe::Color border_color, GLuint target = GL_TEXTURE_2D);
    /// \param k
    /// \return
    GLuint &operator[](const GLuint &k) { return parameters_[k]; }
    /// Applies texture parameters to current bound texture object
    void apply() const;

  private:
    std::map<GLuint, GLuint> parameters_;
    GLenum target_{GL_TEXTURE_2D};
    bool using_border_{false};
    circe::Color border_color_;
  };
  // ***********************************************************************
  //                          STATIC METHODS
  // ***********************************************************************
  ///
  /// \param path
  /// \param input_options
  /// \param output_options
  /// \return Texture object
  static Texture fromFile(const hermes::Path &path,
                          circe::texture_options input_options = circe::texture_options::none,
                          circe::texture_options output_options = circe::texture_options::none);
  /// Load images into a cube map texture
  /// \note This function expects the face sequence: right,
  ///   left, top, bottom, front, back
  /// \param face_paths
  /// \return
  static Texture fromFiles(const std::vector<hermes::Path> &face_paths);

  static Texture fromTexture(const Texture &texture,
                             circe::texture_options output_options = circe::texture_options::none);
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  Texture();
  /// Parameter constructor
  /// \param a texture attributes
  /// \param p texture parameters
  /// \param data
  explicit Texture(const Attributes &a, const void *data = nullptr);
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
  void bind() const;
  void unbind() const;
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
  //                           DATA
  // ***********************************************************************
  /// retrieve texture pixel data
  /// \return list of pixels by row major
  [[nodiscard]] std::vector<unsigned char> texels() const;
  [[nodiscard]] std::vector<unsigned char> texels(hermes::index2 offset, hermes::size2 size) const;
  // ***********************************************************************
  //                           METRICS
  // ***********************************************************************
  /// \return texture resolution in texels
  [[nodiscard]] hermes::size3 size() const;
  /// \return opengl object associated to this texture
  [[nodiscard]] GLuint textureObjectId() const;
  /// \return
  // ***********************************************************************
  //                           ATTRIBUTES
  // ***********************************************************************
  /// \return target texture (ex: GL_TEXTURE_3D)
  [[nodiscard]] GLenum target() const;
  /// \return the color components in the texture (ex: GL_RGBA8)
  [[nodiscard]] GLint internalFormat() const;
  /// \return format of pixel data (ex: GL_RGBA, GL_RED_INTEGER, ...)
  [[nodiscard]] GLenum format() const;
  /// \return data type of pixel data (ex: GL_UNSIGNED_BYTE, GL_FLOAT)
  [[nodiscard]] GLenum type() const;

  /// Textures can be copied, only moved
  /// \param a texture attributes
  /// \param p texture parameters
  /// \param data
  virtual void set(const Attributes &a);
  /// \param texels texture content
  void setTexels(const void *texels) const;
  ///
  /// \param texels
  /// \param target
  void setTexels(GLenum target, const void *texels) const;
  /// \param new_size
  void resize(const hermes::size3 &new_size);
  /// \param new_size
  void resize(const hermes::size2 &new_size);
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
  Attributes attributes_;
  GLuint texture_object_{0};  //!< open gl's texture handle identifier
};

} // namespace circe

#endif // CIRCE_IO_TEXTURE_H
