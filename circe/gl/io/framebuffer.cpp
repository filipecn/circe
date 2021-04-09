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

#include <circe/gl/io/framebuffer.h>

namespace circe::gl {

Framebuffer::Framebuffer() = default;

Framebuffer::Framebuffer(const ponos::size3 &resolution) {
  resize(resolution);
}

Framebuffer::Framebuffer(const ponos::size2 &resolution) {
  resize(resolution);
}

Framebuffer::~Framebuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  if (framebuffer_object_)
    glDeleteFramebuffers(1, &framebuffer_object_);
}

void Framebuffer::resize(const ponos::size2 &resolution) {
  resize({resolution.width, resolution.height, 0});
}

void Framebuffer::resize(const ponos::size3 &resolution) {
  size_in_pixels_ = resolution;
  // delete any previous buffers
  if (framebuffer_object_)
    glDeleteFramebuffers(1, &framebuffer_object_);
  if (render_buffer_object_)
    glDeleteRenderbuffers(1, &render_buffer_object_);
  // create frame buffer
  glGenFramebuffers(1, &framebuffer_object_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_);
  // create render buffer
  glGenRenderbuffers(1, &render_buffer_object_);
  glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_object_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution.width, resolution.height);
  // attach the renderbuffer to depth attachment point
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, // 1. fbo target: GL_FRAMEBUFFER
                            GL_DEPTH_ATTACHMENT, // 2. attachment point
                            GL_RENDERBUFFER, // 3. rbo target: GL_RENDERBUFFER
                            render_buffer_object_); // 4. rbo ID
  CHECK_GL_ERRORS;
  CHECK_FRAMEBUFFER;
  // unbind before leave
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachColorBuffer(GLuint textureId, GLenum target,
                                    GLenum attachmentPoint) const {
  // bind framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_);
  // attach the texture to FBO color attachment point
  glFramebufferTexture2D(GL_FRAMEBUFFER,  // 1. fbo target: GL_FRAMEBUFFER
                         attachmentPoint, // 2. attachment point
                         target,          // 3. tex target: GL_TEXTURE_2D
                         textureId,       // 4. tex ID
                         0);              // 5. mipmap level: 0(base)
  CHECK_GL_ERRORS;
  CHECK_FRAMEBUFFER;
  // unbind before leave
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachTexture(const Texture &texture, GLenum attachment_point) const {
  texture.bind();
  attachColorBuffer(texture.textureObjectId(), texture.target(), attachment_point);
  texture.unbind();
}

void Framebuffer::enable() const {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_);
  glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_object_);
}

void Framebuffer::disable() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Framebuffer::render(const std::function<void()> &render_function) const {
  enable();
  glViewport(0, 0, static_cast<GLsizei>(size_in_pixels_.width),
             static_cast<GLsizei>(size_in_pixels_.height));
  glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
  glClear(clear_bitfield_mask);
  render_function();
  disable();
}

} // circe namespace
