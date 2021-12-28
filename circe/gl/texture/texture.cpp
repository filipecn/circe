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
#include <circe/gl/io/framebuffer.h>
#include <circe/gl/scene/scene_model.h>
#include <circe/gl/graphics/shader.h>
#include <circe/scene/shapes.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace circe::gl {

Texture unfoldCubemap(const Texture &cubemap, texture_options output_options) {
  bool output_is_equirectangular = circe::testMaskBit(output_options, circe::texture_options::equirectangular);
  Texture output;
  output.setTarget(GL_TEXTURE_2D);
  output.setInternalFormat(cubemap.internalFormat());
  output.setFormat(cubemap.format());
  output.setType(cubemap.type());

  SceneModel quad;
  quad = Shapes::box({{-1, -1}, {1, 1}}, shape_options::uv);
  std::string vs = "#version 330 core                                                                               \n"
                   "layout (location = 0) in vec3 aPos;                                                             \n"
                   "layout (location = 1) in vec2 aTexCoords;                                                       \n"
                   "out vec2 uv;                                                                                    \n"
                   "void main() {                                                                                   \n"
                   "    uv = aTexCoords;                                                                            \n"
                   "    gl_Position = vec4(aPos, 1.0);                                                              \n"
                   "}";
  std::string fs;
  if (output_is_equirectangular) {
    fs = "#version 330 core                                                                                         \n"
         "uniform samplerCube cubemap;                                                                              \n"
         "in vec2 uv;                                                                                               \n"
         "out vec4 FragColor;                                                                                       \n"
         "void main() {                                                                                             \n"
         "  float phi = uv.x * 3.1415 * 2;                                                                          \n"
         "  float theta = (-uv.y + 0.5) * 3.1415;                                                                   \n"
         "  vec3 dir = vec3(cos(phi)*cos(theta),sin(theta),sin(phi)*cos(theta));                                    \n"
         "  vec3 color = texture(cubemap, dir).rgb;                                                                 \n"
         "  color = color / (color + vec3(1.0));                                                                    \n"
         "  color = pow(color, vec3(1.0/2.2));                                                                      \n"
         "  FragColor = vec4(color, 1.0);                                                                           \n"
         "}";
    output.resize({cubemap.size().width * 2, cubemap.size().height * 1});
  } else {
    fs = "#version 330 core                                                                                         \n"
         "uniform samplerCube cubemap;                                                                              \n"
         "in vec2 uv;                                                                                               \n"
         "out vec4 FragColor;                                                                                       \n"
         "void main() {                                                                                             \n"
         "  vec3 color = vec3(0.0,0.0,0.0);                                                                         \n"
         "  vec2 lp = vec2(mod(uv.x * 4, 1), mod(uv.y * 3, 1));                                                     \n"
         "  vec2 wp = vec2(uv.x * 4, uv.y * 3);                                                                     \n"
         "  if(wp.x > 1 && wp.x < 2) {                                                                              \n"
         "    if(wp.y > 2) // top (+y)                                                                              \n"
         "      color = texture(cubemap, vec3(-lp.x * 2 + 1, 1, -lp.y * 2 + 1)).rgb;                                \n"
         "    else if(wp.y < 1) // bottom (-y)                                                                      \n"
         "      color = texture(cubemap, vec3(-lp.x * 2 + 1, -1, -lp.y * 2 + 1)).rgb;                               \n"
         "    else // front (-z)                                                                                    \n"
         "      color = texture(cubemap, vec3(-lp.x * 2 + 1,lp.y * 2 - 1, -1)).rgb;                                 \n"
         "  } else if(wp.y > 1 && wp.y < 2) {                                                                       \n"
         "    if(wp.x < 1) // left (+x)                                                                             \n"
         "      color = texture(cubemap, vec3(1,lp.y * 2 - 1,-lp.x * 2 + 1)).rgb;                                   \n"
         "    else if(wp.x < 3) // right (-x)                                                                       \n"
         "      color = texture(cubemap, vec3(-1,lp.y * 2 - 1,lp.x * 2 - 1)).rgb;                                  \n"
         "    else // back (+z)                                                                                     \n"
         "      color = texture(cubemap, vec3(lp.x - 2 + 1,lp.y * 2 - 1, 1)).rgb;                                  \n"
         "  } else { FragColor = vec4(0.0); return; }                                                               \n"
         "  color = color / (color + vec3(1.0));                                                                    \n"
         "  color = pow(color, vec3(1.0/2.2));                                                                      \n"
         "  FragColor = vec4(color, 1.0);                                                                           \n"
         "}";
    output.resize({cubemap.size().width * 4, cubemap.size().height * 3});
  }

  output.bind();
  Texture::View().apply();

  ////////////////////////////// build shader /////////////////////////////////////////////////////////////////////////
  Program program;
  program.attach(Shader(GL_VERTEX_SHADER, vs));
  program.attach(Shader(GL_FRAGMENT_SHADER, fs));
  if (!program.link())
    std::cerr << "failed to compile unfold cubemap shader!\n" << program.err << std::endl;

  Framebuffer framebuffer;
  framebuffer.setRenderBufferStorageInternalFormat(GL_DEPTH_COMPONENT24);
  framebuffer.resize(output.size());
  framebuffer.enable();
  glViewport(0, 0, output.size().width, output.size().height);
  framebuffer.attachTexture(output);
  framebuffer.enable();

  cubemap.bind(GL_TEXTURE0);
  program.use();
  program.setUniform("cubemap", 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  quad.draw();
  Framebuffer::disable();
  return output;
}

Texture convertToCubemap(const Texture &input, texture_options input_options, hermes::size2 resolution) {
  bool input_is_hdr = circe::testMaskBit(input_options, circe::texture_options::hdr);
  bool input_is_equirectangular = testMaskBit(input_options, texture_options::equirectangular);
  std::string vs;
  std::string fs;
  Program program;
  if (input_is_equirectangular) {
    vs = "#version 330 core\n"
         "layout (location = 0) in vec3 aPos;\n"
         "out vec3 localPos;\n"
         "uniform mat4 projection;\n"
         "uniform mat4 view;\n"
         "void main() {\n"
         "    localPos = aPos;  \n"
         "    gl_Position =  projection * view * vec4(localPos, 1.0);\n"
         "}";

    fs = "#version 330 core\n"
         "out vec4 FragColor;\n"
         "in vec3 localPos;\n"
         "uniform sampler2D equirectangularMap;\n"
         "const vec2 invAtan = vec2(0.1591, 0.3183);\n"
         "vec2 SampleSphericalMap(vec3 v) {\n"
         "    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));\n"
         "    uv *= invAtan;\n"
         "    uv += 0.5;\n"
         "    return uv;\n"
         "}\n"
         "void main() {\n"
         "    vec2 uv = SampleSphericalMap(normalize(localPos));\n"
         "    vec3 color = texture(equirectangularMap, uv).rgb;\n"
         "    FragColor = vec4(color, 1.0);\n"
         "}";
    program.attach(Shader(GL_VERTEX_SHADER, vs));
    program.attach(Shader(GL_FRAGMENT_SHADER, fs));
    if (!program.link())
      std::cerr << "failed to compile equirectangular map shader!\n" << program.err << std::endl;
    program.use();
    program.setUniform("equirectangularMap", 0);
  }

  Texture cubemap;
  cubemap.setTarget(GL_TEXTURE_CUBE_MAP);
  cubemap.setFormat(GL_RGB);
  if (input_is_hdr) {
    cubemap.setInternalFormat(GL_RGBA16F);
    cubemap.setType(GL_FLOAT);
  }

  cubemap.resize(resolution);
  cubemap.bind();

  circe::gl::Texture::View parameters(GL_TEXTURE_CUBE_MAP);
  parameters[GL_TEXTURE_MIN_FILTER] = GL_LINEAR;
  parameters[GL_TEXTURE_MAG_FILTER] = GL_LINEAR;
  parameters[GL_TEXTURE_WRAP_S] = GL_CLAMP_TO_EDGE;
  parameters[GL_TEXTURE_WRAP_T] = GL_CLAMP_TO_EDGE;
  parameters[GL_TEXTURE_WRAP_R] = GL_CLAMP_TO_EDGE;
  parameters.apply();

  // render cubemap faces
  auto projection = hermes::Transform::perspective(90, 1, 0.1, 10);
  hermes::Transform views[] = {
      hermes::Transform::lookAt({}, {1, 0, 0}, {0, -1, 0}),
      hermes::Transform::lookAt({}, {-1, 0, 0}, {0, -1, 0}),
      hermes::Transform::lookAt({}, {0, 1, 0}, {0, 0, -1}),
      hermes::Transform::lookAt({}, {0, -1, 0}, {0, 0, 1}),
      hermes::Transform::lookAt({}, {0, 0, -1}, {0, -1, 0}),
      hermes::Transform::lookAt({}, {0, 0, 1}, {0, -1, 0}),
  };

  Framebuffer frambuffer;
  frambuffer.setRenderBufferStorageInternalFormat(GL_DEPTH_COMPONENT24);
  frambuffer.resize(resolution);
  frambuffer.enable();
  glViewport(0, 0, resolution.width, resolution.height);

  SceneModel cube;
  cube = circe::Shapes::box({{-1, -1, -1}, {1, 1, 1}});

  input.bind();
  program.use();
  program.setUniform("projection", projection);
  for (u32 i = 0; i < 6; ++i) {
    program.setUniform("view", views[i]);
    frambuffer.attachColorBuffer(cubemap.textureObjectId(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
    frambuffer.enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cube.draw();
  }
  Framebuffer::disable();
  return cubemap;
}

Texture::Atlas::Atlas() = default;

Texture::Atlas::~Atlas() = default;

[[nodiscard]] const hermes::size2 &Texture::Atlas::size_in_texels() const {
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

const hermes::bbox2 &Texture::Atlas::uv(size_t i) const {
  return uvs_[i];
}

Texture::View::View(GLuint target) : target_{target} {
  parameters_[GL_TEXTURE_WRAP_S] = GL_CLAMP_TO_EDGE;
  parameters_[GL_TEXTURE_WRAP_T] = GL_CLAMP_TO_EDGE;
  parameters_[GL_TEXTURE_WRAP_R] = GL_CLAMP_TO_EDGE;
  parameters_[GL_TEXTURE_MIN_FILTER] = GL_LINEAR;
  parameters_[GL_TEXTURE_MAG_FILTER] = GL_LINEAR;
//  parameters_[GL_TEXTURE_BASE_LEVEL] = 0;
//  parameters_[GL_TEXTURE_MAX_LEVEL] = 0;
}

Texture::View::View(circe::Color border_color, GLuint target) : Texture::View::View(target) {
  using_border_ = true;
  border_color_ = border_color;
  parameters_[GL_TEXTURE_WRAP_S] = GL_CLAMP_TO_BORDER;
  parameters_[GL_TEXTURE_WRAP_T] = GL_CLAMP_TO_BORDER;
  parameters_[GL_TEXTURE_WRAP_R] = GL_CLAMP_TO_BORDER;
}

void Texture::View::apply() const {
  for (auto &parameter : parameters_)
    glTexParameteri(target_, parameter.first, parameter.second);
  if (using_border_)
    glTexParameterfv(target_, GL_TEXTURE_BORDER_COLOR, border_color_.asArray());
}

Texture Texture::fromFile(const hermes::Path &path,
                          circe::texture_options input_options,
                          circe::texture_options output_options) {
  // check input options
  bool input_is_hdr = circe::testMaskBit(input_options, circe::texture_options::hdr);
  // check output options
  bool output_is_cubemap = circe::testMaskBit(output_options, circe::texture_options::cubemap);
  // texture object
  Texture texture;
  // read file
  int width, height, channel_count;
  void *data;
  if (input_is_hdr) {
    stbi_set_flip_vertically_on_load(true);
    data = stbi_loadf(path.fullName().c_str(), &width, &height, &channel_count, 0);
    texture.attributes_.target = GL_TEXTURE_2D;
    texture.attributes_.internal_format = GL_RGB16F;
    texture.attributes_.format = (channel_count == 3) ? GL_RGB : GL_RGBA;
    texture.attributes_.type = GL_FLOAT;
  } else {
    data = stbi_load(path.fullName().c_str(), &width, &height, &channel_count, 0);
    texture.attributes_.target = GL_TEXTURE_2D;
    texture.attributes_.format = (channel_count == 3) ? GL_RGB : GL_RGBA;
    texture.attributes_.internal_format = GL_RGB;
    texture.attributes_.type = GL_UNSIGNED_BYTE;
  }
  if (!data) {
    std::cerr << "Failed to load texture from file " << path << std::endl;
    return Texture();
  }
  // init texture
  texture.attributes_.size_in_texels = hermes::size3(width, height, 1);
  texture.setTexels(data);
  texture.bind();

  circe::gl::Texture::View().apply();

  texture.unbind();
  stbi_image_free(data);

  if (output_is_cubemap)
    return convertToCubemap(texture, input_options, {512, 512});

  return texture;
}

Texture Texture::fromFiles(const std::vector<hermes::Path> &face_paths) {
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
    texture.setTexels(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, data);
    CHECK_GL_ERRORS;
    stbi_image_free(data);
  }
  texture.unbind();
  return texture;
}

Texture Texture::fromTexture(const Texture &texture, circe::texture_options output_options) {
  // check input options
  bool input_is_cubemap = texture.target() == GL_TEXTURE_CUBE_MAP;
  // check output options
  bool output_is_cubemap = circe::testMaskBit(output_options, circe::texture_options::cubemap);
  bool output_is_equirectangular = circe::testMaskBit(output_options, circe::texture_options::equirectangular);

  if (input_is_cubemap && !output_is_cubemap)
    return unfoldCubemap(texture, output_options);
  return Texture();
}

Texture::Texture() {
  glGenTextures(1, &texture_object_);
  HERMES_ASSERT(texture_object_);
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
  else if (attributes_.target == GL_TEXTURE_CUBE_MAP)
    for (u32 i = 0; i < 6; ++i)
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, attributes_.internal_format, attributes_.size_in_texels.width,
                   attributes_.size_in_texels.height, 0, attributes_.format, attributes_.type,
                   texels);
  else
    glTexImage2D(GL_TEXTURE_2D, 0, attributes_.internal_format, attributes_.size_in_texels.width,
                 attributes_.size_in_texels.height, 0, attributes_.format, attributes_.type,
                 texels);

  CHECK_GL_ERRORS;
  glBindTexture(attributes_.target, 0);
}

void Texture::setTexels(GLenum target, const void *texels) const {
  /// bind texture
  glBindTexture(attributes_.target, texture_object_);
  glTexImage2D(target, 0, attributes_.internal_format, attributes_.size_in_texels.width,
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

hermes::size3 Texture::size() const {
  return attributes_.size_in_texels;
}

GLuint Texture::textureObjectId() const { return texture_object_; }

GLenum Texture::target() const { return attributes_.target; }

GLint Texture::internalFormat() const { return attributes_.internal_format; }

GLenum Texture::format() const { return attributes_.format; }

GLenum Texture::type() const { return attributes_.type; }

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
      case GL_UNSIGNED_INT:out << reinterpret_cast<u32 *>(data)[(int) (j * width + i)] << ",";
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

  size_t element_size = 1;
  if(attributes_.type == GL_FLOAT)
    element_size = 4;
  std::vector<unsigned char> data(element_size * 4 * width * height, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(attributes_.target, texture_object_);
  glGetTexImage(attributes_.target, 0, attributes_.format, attributes_.type, &data[0]);
  CHECK_GL_ERRORS;
  return data;
}

std::vector<unsigned char> Texture::texels(hermes::index2 offset, hermes::size2 size) const {
  if (offset.i < 0 || offset.j < 0 || offset.i + size.width >= attributes_.size_in_texels.width
      || offset.j + size.height >= attributes_.size_in_texels.height)
    return {};

  std::vector<unsigned char> data(4 * size.total());

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(attributes_.target, texture_object_);
  glGetTextureSubImage(attributes_.target, 0, offset.i, offset.j, 0, size.width, size.height, 1,
                       GL_RGBA, GL_UNSIGNED_BYTE, data.size(), &data[0]);
  CHECK_GL_ERRORS;
  return data;
}

void Texture::resize(const hermes::size3 &new_size) {
  attributes_.size_in_texels = new_size;
  setTexels(nullptr);
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
void Texture::resize(const hermes::size2 &new_size) {
  attributes_.size_in_texels.width = new_size.width;
  attributes_.size_in_texels.height = new_size.height;
  attributes_.size_in_texels.depth = 1;
  setTexels(nullptr);
}

} // namespace circe
