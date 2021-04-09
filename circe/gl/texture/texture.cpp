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

#include <circe/gl/texture/texture.h>
#include <circe/gl/utils/open_gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace circe::gl {

Texture::Atlas::Atlas() = default;

Texture::Atlas::~Atlas() = default;

[[nodiscard]] const ponos::size2 &Texture::Atlas::size_in_texels() const {
  return size_in_texels_;
}

size_t Texture::Atlas::push(const Texture::Atlas::Region &region) {
  auto old_size = size_in_texels_;
  auto upper = region.offset + region.size;
  size_in_texels_.width = std::max(size_in_texels_.width, upper.width);
  size_in_texels_.height = std::max(size_in_texels_.height, upper.height);
  if (size_in_texels_ != old_size)
    updateUVs();
  regions_.emplace_back(region);
  uvs_.push_back({{
                      static_cast<real_t>( region.offset.width) /
                          size_in_texels_.width,
                      static_cast<real_t>( region.offset.height) /
                          size_in_texels_.height
                  },
                  {
                      static_cast<real_t>( upper.width) /
                          size_in_texels_.width,
                      static_cast<real_t>( upper.height) /
                          size_in_texels_.height
                  }});
  return regions_.size() - 1;
}

void Texture::Atlas::updateUVs() {
  for (size_t i = 0; i < uvs_.size(); ++i) {
    uvs_[i].lower =
        {static_cast<real_t>( regions_[i].offset.width) /
            size_in_texels_.width,
         static_cast<real_t>( regions_[i].offset.height) /
             size_in_texels_.height};
    uvs_[i].upper =
        {static_cast<real_t>( regions_[i].offset.width + regions_[i].size.width) /
            size_in_texels_.width,
         static_cast<real_t>( regions_[i].offset.height + regions_[i].size.height) /
             size_in_texels_.height};
  }
}

const Texture::Atlas::Region &Texture::Atlas::operator[](size_t i) const {
  return regions_[i];
}

Texture::Atlas::Region &Texture::Atlas::operator[](size_t i) {
  return regions_[i];
}

const ponos::bbox2 &Texture::Atlas::uv(size_t i) const {
  return uvs_[i];
}

Texture::View::View(GLuint target) : target_{target} {
  parameters_[GL_TEXTURE_WRAP_S] = GL_CLAMP_TO_EDGE;
  parameters_[GL_TEXTURE_WRAP_T] = GL_CLAMP_TO_EDGE;
  parameters_[GL_TEXTURE_MIN_FILTER] = GL_LINEAR;
  parameters_[GL_TEXTURE_MAG_FILTER] = GL_LINEAR;
  parameters_[GL_TEXTURE_BASE_LEVEL] = 0;
  parameters_[GL_TEXTURE_MAX_LEVEL] = 0;

  if (target == GL_TEXTURE_3D)
    parameters_[GL_TEXTURE_WRAP_R] = GL_CLAMP_TO_EDGE;
}

Texture::View::View(circe::Color border_color, GLuint target) : Texture::View::View(target) {
  using_border_ = true;
  border_color_ = border_color;
  parameters_[GL_TEXTURE_WRAP_S] = GL_CLAMP_TO_BORDER;
  parameters_[GL_TEXTURE_WRAP_T] = GL_CLAMP_TO_BORDER;
  if (target == GL_TEXTURE_3D)
    parameters_[GL_TEXTURE_WRAP_R] = GL_CLAMP_TO_BORDER;
}

void Texture::View::apply() const {
  for (auto &parameter : parameters_)
    glTexParameteri(target_, parameter.first, parameter.second);
  if (using_border_)
    glTexParameterfv(target_, GL_TEXTURE_BORDER_COLOR, border_color_.asArray());
}

Texture Texture::fromFile(const ponos::Path &path) {
  int width, height, channel_count;
  unsigned char *data = stbi_load(path.fullName().c_str(), &width, &height, &channel_count, 0);
  if (!data) {
    std::cerr << "Failed to load texture from file " << path << std::endl;
    return Texture();
  }
  Texture texture;
  texture.attributes_.target = GL_TEXTURE_2D;
  texture.attributes_.size_in_texels = ponos::size3(width, height, 1);
  texture.attributes_.type = GL_UNSIGNED_BYTE;
  texture.attributes_.format = (channel_count == 3) ? GL_RGB : GL_RGBA;
  texture.attributes_.internal_format = GL_RGB;
  texture.setTexels(data);
  texture.bind();
  circe::gl::Texture::View().apply();
  texture.unbind();
  stbi_image_free(data);
  return texture;
}

Texture Texture::fromFiles(const std::vector<ponos::Path> &face_paths) {
  Texture texture;
  // resize cube map
  texture.setTarget(GL_TEXTURE_CUBE_MAP);
  texture.setType(GL_UNSIGNED_BYTE);
  texture.attributes_.size_in_texels.depth = 1;
  // bind texture
  texture.bind();
  // load faces
  int width, height, channel_count;
  for (size_t i = 0; i < face_paths.size(); ++i) {
    unsigned char *data = stbi_load(face_paths[i].fullName().c_str(), &width, &height, &channel_count, 0);
    if (!data) {
      // TODO report error
      std::cerr << "failed to load cubemap face texture\n";
      return Texture();
    }
    texture.attributes_.size_in_texels.width = width;
    texture.attributes_.size_in_texels.height = height;
    texture.attributes_.format = GL_RGB;
    texture.attributes_.internal_format = GL_RGB;
    // store texels
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                 0, texture.attributes_.internal_format, texture.attributes_.size_in_texels.width,
                 texture.attributes_.size_in_texels.height, 0, texture.attributes_.format, texture.attributes_.type,
                 data);
    CHECK_GL_ERRORS;
    stbi_image_free(data);
  }
  texture.unbind();
  return texture;
}

Texture::Texture() {
  glGenTextures(1, &texture_object_);
  ASSERT(texture_object_);
}

Texture::Texture(const Texture::Attributes &a, const void *data) : Texture() {
  attributes_ = a;
  setTexels(data);
  glBindTexture(attributes_.target, 0);
}

Texture::Texture(const Texture &other) : Texture() {
  attributes_ = other.attributes_;
  setTexels(nullptr);
  glBindTexture(attributes_.target, 0);
}

Texture::Texture(Texture &&other) noexcept {
  if (texture_object_)
    glDeleteTextures(1, &texture_object_);
  texture_object_ = other.texture_object_;
  attributes_ = other.attributes_;
  other.texture_object_ = 0;
}

Texture::~Texture() {
  glDeleteTextures(1, &texture_object_);
}

void Texture::set(const Texture::Attributes &a) {
  attributes_ = a;
  setTexels(nullptr);
  glBindTexture(attributes_.target, 0);
}

Texture &Texture::operator=(const Texture &other) {
  set(other.attributes_);
  return *this;
}

Texture &Texture::operator=(Texture &&other) noexcept {
  if (texture_object_)
    glDeleteTextures(1, &texture_object_);
  texture_object_ = other.texture_object_;
  attributes_ = other.attributes_;
  other.texture_object_ = 0;
  return *this;
}

void Texture::setTexels(const void *texels) const {
  /// bind texture
  glBindTexture(attributes_.target, texture_object_);
  if (attributes_.target == GL_TEXTURE_3D)
    glTexImage3D(GL_TEXTURE_3D, 0, attributes_.internal_format, attributes_.size_in_texels.width,
                 attributes_.size_in_texels.height, attributes_.size_in_texels.depth, 0, attributes_.format,
                 attributes_.type, texels);
  else
    glTexImage2D(GL_TEXTURE_2D, 0, attributes_.internal_format, attributes_.size_in_texels.width,
                 attributes_.size_in_texels.height, 0, attributes_.format, attributes_.type,
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

void Texture::bind() const {
  glBindTexture(attributes_.target, texture_object_);
}

void Texture::unbind() const {
  glBindTexture(attributes_.target, 0);
}

void Texture::bind(GLenum t) const {
  glActiveTexture(t);
  glBindTexture(attributes_.target, texture_object_);
}

void Texture::bindImage(GLenum t) const {
  glActiveTexture(t);
  glBindImageTexture(0, texture_object_, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                     attributes_.internal_format);
  CHECK_GL_ERRORS;
}

ponos::size3 Texture::size() const {
  return attributes_.size_in_texels;
}

GLuint Texture::textureObjectId() const { return texture_object_; }

GLenum Texture::target() const { return attributes_.target; }

std::ostream &operator<<(std::ostream &out, Texture &pt) {
  auto width = static_cast<int>(pt.attributes_.size_in_texels.width);
  auto height = static_cast<int>(pt.attributes_.size_in_texels.height);

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
  auto width = static_cast<int>(attributes_.size_in_texels.width);
  auto height = static_cast<int>(attributes_.size_in_texels.height);

  std::vector<unsigned char> data(4 * width * height, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(attributes_.target, texture_object_);
  glGetTexImage(attributes_.target, 0, attributes_.format, attributes_.type, &data[0]);
  CHECK_GL_ERRORS;
  return data;
}

void Texture::resize(const ponos::size3 &new_size) {
  attributes_.size_in_texels = new_size;
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

void Texture::setTarget(GLenum _target) {
  attributes_.target = _target;
}

void Texture::resize(const ponos::size2 &new_size) {
  attributes_.size_in_texels.width = new_size.width;
  attributes_.size_in_texels.height = new_size.height;
  attributes_.size_in_texels.depth = 1;
  setTexels(nullptr);
}
} // namespace circe
