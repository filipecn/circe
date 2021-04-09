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

#include <circe/gl/texture/framebuffer_texture.h>
#include <circe/gl/utils/open_gl.h>

namespace circe::gl {

FramebufferTexture::FramebufferTexture() : Texture() {}

FramebufferTexture::FramebufferTexture(const Texture::Attributes &a)
    : Texture(a) {
  framebuffer_.resize(attributes_.size_in_texels);
  framebuffer_.attachColorBuffer(texture_object_, attributes_.target);
  Framebuffer::disable();
}

FramebufferTexture::~FramebufferTexture() {
  if (texture_object_)
    glDeleteTextures(1, &texture_object_);
}

void FramebufferTexture::render(const std::function<void()> &f) {
  framebuffer_.enable();
  glViewport(0, 0, static_cast<GLsizei>(attributes_.size_in_texels.width),
             static_cast<GLsizei>(attributes_.size_in_texels.height));
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  f();
  Framebuffer::disable();
}

std::ostream &operator<<(std::ostream &out, FramebufferTexture &pt) {
  int width = pt.attributes_.size_in_texels.width;
  int height = pt.attributes_.size_in_texels.height;

  auto *data = new unsigned char[(int) (width * height)];

  for (int i(0); i < width; ++i) {
    for (int j(0); j < height; ++j) {
      data[j * width + i] = 0;
    }
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(pt.attributes_.target, pt.texture_object_);
  glGetTexImage(pt.attributes_.target, 0, pt.attributes_.format,
                pt.attributes_.type, data);

  CHECK_GL_ERRORS;

  out << width << " " << height << std::endl;

  for (int j(height - 1); j >= 0; --j) {
    for (int i(0); i < width; ++i) {
      out << (int) data[(int) (j * width + i)] << ",";
    }
    out << std::endl;
  }
  return out;
}

void FramebufferTexture::set(const Texture::Attributes &a) {
  Texture::set(a);
  framebuffer_.resize(attributes_.size_in_texels);
  framebuffer_.attachColorBuffer(texture_object_, attributes_.target);
  Framebuffer::disable();
}

} // namespace circe
