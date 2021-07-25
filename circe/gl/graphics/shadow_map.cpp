/// Copyright (c) 2020, FilipeCN.
///
/// The MIT License (MIT)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.
///
///\file shadow_map.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-08-09
///
///\brief

#include <circe/gl/graphics/shadow_map.h>

namespace circe::gl {

ShadowMap::ShadowMap(const hermes::size2 &size) : size_(size),
                                                 projection_transform_{
                                                     hermes::Transform::ortho(-10, 10, -10, 10, -10, 10)} {
  // setup shader program
  Shader vertex_shader(GL_VERTEX_SHADER, "#version 430 core\n"
                                         "layout (location = 0) in vec3 position;\n"
                                         "layout (location = 1) uniform mat4 lightSpaceMatrix;\n"
                                         "layout (location = 2) uniform mat4 model;\n"
                                         "void main()\n"
                                         "{ gl_Position = lightSpaceMatrix * model * vec4(position, 1.0); }");
  Shader fragment_shader(GL_FRAGMENT_SHADER, "#version 430 core\nvoid main(){ "
                                             "gl_FragDepth = gl_FragCoord.z;"
                                             "//gl_FragDepth += gl_FrontFacing ? 0.01 : 0.0;\n }\n");
  program_.attach(vertex_shader);
  program_.attach(fragment_shader);
  if (!program_.link()) {
    std::cerr << program_.err << std::endl;
    exit(-1);
  }
  // setup depth texture attributes and parameters
  Texture::Attributes attributes;
  attributes.size_in_texels = {size.width, size.height, 1};
  attributes.target = GL_TEXTURE_2D;
  attributes.internal_format = GL_DEPTH_COMPONENT;
  attributes.format = GL_DEPTH_COMPONENT;
  attributes.type = GL_FLOAT;
  Texture::View parameters(Color::White());
  parameters[GL_TEXTURE_MIN_FILTER] = GL_NEAREST;
  parameters[GL_TEXTURE_MAG_FILTER] = GL_NEAREST;
  parameters[GL_TEXTURE_WRAP_S] = GL_CLAMP_TO_BORDER;
  parameters[GL_TEXTURE_WRAP_T] = GL_CLAMP_TO_BORDER;
  // resize depth texture
  depth_map_.set(attributes);
  depth_map_.bind();
  parameters.apply();
  // resize buffer
  depth_buffer_.resize(size);
  depth_buffer_.attachColorBuffer(depth_map_.textureObjectId(), depth_map_.target(), GL_DEPTH_ATTACHMENT);
  depth_buffer_.enable();
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  // Since we don't need a color buffer and Open GL expects one, we must tell Open GL:
  circe::gl::Framebuffer::disable();
  CHECK_GL_ERRORS;
}

ShadowMap::~ShadowMap() = default;

void ShadowMap::render(const std::function<void(const Program &)> &f) {
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, size_.width, size_.height);
  depth_buffer_.enable();
  glClear(GL_DEPTH_BUFFER_BIT);
  depth_map_.bind(GL_TEXTURE0);
  program_.use();
  program_.setUniform("lightSpaceMatrix", light_transform_);
  program_.setUniform("model", hermes::Transform());
  if (f)
    f(program_);
  circe::gl::Framebuffer::disable();
}

void ShadowMap::bind() const {
  glBindTexture(GL_TEXTURE_2D, depth_map_.textureObjectId());
}

void ShadowMap::setLight(const Light &light) {
  light_ = light;
  light_transform_ = projection_transform_ * hermes::Transform::lookAt(hermes::point3() + light_.direction);
}

const hermes::Transform &ShadowMap::light_transform() const { return light_transform_; }

const Texture &ShadowMap::depthMap() const {
  return depth_map_;
}

}