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

#ifndef CIRCE_IO_FRAMEBUFFER_H
#define CIRCE_IO_FRAMEBUFFER_H

#include <circe/gl/io/texture_parameters.h>
#include <circe/gl/utils/open_gl.h>

#include <ponos/ponos.h>

namespace circe::gl {

class Framebuffer {
public:
  Framebuffer();
  Framebuffer(uint w, uint h, uint d = 0);
  virtual ~Framebuffer();

  void set(uint w, uint h, uint d = 0);
  void enable() const;
  static void disable();
  void attachColorBuffer(GLuint textureId, GLenum target,
                         GLenum attachmentPoint = GL_COLOR_ATTACHMENT0) const;

private:
  uint width, height, depth;
  GLuint framebufferObject;
  GLuint renderBufferObject;
};

} // circe namespace

#endif // CIRCE_IO_FRAMEBUFFER_H
