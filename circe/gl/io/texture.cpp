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

#include <circe/gl/io/texture.h>
#include <circe/gl/utils/open_gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace circe::gl {

Texture Texture::fromFile(const ponos::Path &path) {
  int width, height, channel_count;
  unsigned char *data = stbi_load(path.fullName().c_str(), &width, &height, &channel_count, 0);
  if (!data)
    return Texture();
  Texture texture;
  texture.attributes_.target = GL_TEXTURE_2D;
  texture.attributes_.width = width;
  texture.attributes_.height = height;
  texture.attributes_.depth = 1;
  texture.attributes_.type = GL_UNSIGNED_BYTE;
  texture.attributes_.format = (channel_count == 3) ? GL_RGB : GL_RGBA;
  texture.attributes_.internal_format = GL_RGB;
  texture.setTexels(data);
  stbi_image_free(data);
  return texture;
}

Texture::Texture() {
  glGenTextures(1, &texture_object_);
  ASSERT(texture_object_);
}

Texture::Texture(const TextureAttributes &a, const TextureParameters &p, const void *data)
    : Texture() {
  set(a, p);
}

Texture::Texture(const Texture &other) {
  set(other.attributes_, other.parameters_);
}

Texture::Texture(Texture &&other) noexcept {
  if (texture_object_)
    glDeleteTextures(1, &texture_object_);
  texture_object_ = other.texture_object_;
  parameters_changed_ = other.parameters_changed_;
  parameters_ = other.parameters_;
  attributes_ = other.attributes_;
  other.texture_object_ = 0;
}

Texture::~Texture() {
  glDeleteTextures(1, &texture_object_);
}

Texture &Texture::operator=(const Texture &other) {
  set(other.attributes_, other.parameters_);
  return *this;
}

Texture &Texture::operator=(Texture &&other) noexcept {
  texture_object_ = other.texture_object_;
  parameters_changed_ = other.parameters_changed_;
  parameters_ = other.parameters_;
  attributes_ = other.attributes_;
  other.texture_object_ = 0;
  return *this;
}

void Texture::set(const TextureAttributes &a, const TextureParameters &p) {
  ASSERT(a.target == p.target);
  attributes_ = a;
  parameters_ = p;
  setTexels(nullptr);
  glBindTexture(attributes_.target, texture_object_);
  parameters_.apply();
  parameters_changed_ = false;
  glBindTexture(attributes_.target, 0);
}

void Texture::setTexels(const void *texels) const {
  /// bind texture
  glBindTexture(attributes_.target, texture_object_);
  if (parameters_.target == GL_TEXTURE_3D)
    glTexImage3D(GL_TEXTURE_3D, 0, attributes_.internal_format, attributes_.width,
                 attributes_.height, attributes_.depth, 0, attributes_.format,
                 attributes_.type, texels);
  else
    glTexImage2D(GL_TEXTURE_2D, 0, attributes_.internal_format, attributes_.width,
                 attributes_.height, 0, attributes_.format, attributes_.type,
                 texels);
  CHECK_GL_ERRORS;
  glBindTexture(attributes_.target, 0);
}

void Texture::generateMipmap() const {
  glBindTexture(attributes_.target, texture_object_);
  glGenerateMipmap(attributes_.target);
  CHECK_GL_ERRORS;
  glBindTexture(attributes_.target, 0);
}

void Texture::bind(GLenum t) const {
  glActiveTexture(t);
  glBindTexture(attributes_.target, texture_object_);
  /// update parameters and attributes if necessary
  if (parameters_changed_) {
    parameters_.apply();
    parameters_changed_ = false;
  }
}

void Texture::bindImage(GLenum t) const {
  glActiveTexture(t);
  glBindImageTexture(0, texture_object_, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                     attributes_.internal_format);
  CHECK_GL_ERRORS;
}

ponos::size3 Texture::size() const {
  return ponos::size3(attributes_.width, attributes_.height, attributes_.depth);
}

GLuint Texture::textureObjectId() const { return texture_object_; }

GLenum Texture::target() const { return parameters_.target; }

std::ostream &operator<<(std::ostream &out, Texture &pt) {
  auto width = static_cast<int>(pt.attributes_.width);
  auto height = static_cast<int>(pt.attributes_.height);

  u8 *data = nullptr;
  auto bytes_per_texel = OpenGL::dataSizeInBytes(pt.attributes_.type);
  auto memory_size = width * height * OpenGL::dataSizeInBytes(pt.attributes_.type);
  data = new u8[memory_size];
  memset(data, 0, memory_size);

  glActiveTexture(GL_TEXTURE0);
  std::cerr << "texture object " << pt.texture_object_ << std::endl;
  out << width << " x " << height << " texels of type " <<
      OpenGL::TypeToStr(pt.attributes_.type) << " (" << bytes_per_texel
      << " bytes per texel)" << std::endl;
  out << "total memory: " << memory_size << " bytes\n";
  glBindTexture(pt.attributes_.target, pt.texture_object_);
  glGetTexImage(pt.attributes_.target, 0, pt.attributes_.format,
                pt.attributes_.type, data);

  CHECK_GL_ERRORS;

  for (int j(height - 1); j >= 0; --j) {
    for (int i(0); i < width; ++i) {
      switch (pt.attributes_.type) {
      case GL_FLOAT:out << reinterpret_cast<f32 *>(data)[(int) (j * width + i)] << ",";
        break;
      default:out << static_cast<int>(data[(int) (j * width + i)]) << ",";
      }
    }
    out << std::endl;
  }

  delete[] data;

  return out;
}

std::vector<unsigned char> Texture::texels() const {
  auto width = static_cast<int>(attributes_.width);
  auto height = static_cast<int>(attributes_.height);

  std::vector<unsigned char> data(4 * width * height, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(attributes_.target, texture_object_);
  glGetTexImage(attributes_.target, 0, attributes_.format, attributes_.type,
                &data[0]);
  CHECK_GL_ERRORS;
  return data;
}

const TextureAttributes &Texture::attributes() const { return attributes_; }

const TextureParameters &Texture::parameters() const { return parameters_; }

void Texture::setParameter(GLuint k, GLuint value) {
  parameters_[k] = value;
  parameters_changed_ = true;
}

void Texture::resize(const ponos::size3 &new_size) {
  attributes_.width = new_size.width;
  attributes_.height = new_size.height;
  attributes_.depth = new_size.depth;
}

void Texture::setInternalFormat(GLint internal_format) {
  attributes_.internal_format = internal_format;
}

void Texture::setFormat(GLint format) {
  attributes_.format = format;
}

void Texture::setType(GLenum type) {
  attributes_.type = type;
}

void Texture::setTarget(GLenum target) {
  attributes_.target = target;
}

void Texture::resize(const ponos::size2 &new_size) {
  attributes_.width = new_size.width;
  attributes_.height = new_size.height;
}

} // namespace circe
