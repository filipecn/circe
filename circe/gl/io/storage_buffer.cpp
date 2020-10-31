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

#include "storage_buffer.h"

namespace circe::gl {

StorageBuffer::~StorageBuffer() { glDeleteBuffers(1, &ssbo); }

StorageBuffer::StorageBuffer(unsigned s, const GLvoid *d) : size(s), ssbo(0) {
  glGenBuffers(1, &ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, size, d, GL_DYNAMIC_COPY);
  CHECK_GL_ERRORS;
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void StorageBuffer::bind() {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  CHECK_GL_ERRORS;
}

GLuint StorageBuffer::id() const { return ssbo; }

void StorageBuffer::write(const void *d) {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  GLvoid *p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size,
                               GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
  CHECK_GL_ERRORS;
  memcpy(p, d, size);
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void StorageBuffer::read(void *d) const {
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  GLvoid *p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
  CHECK_GL_ERRORS;
  memcpy(d, p, size);
  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

} // circe namespace
