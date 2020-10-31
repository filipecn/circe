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
///\file index_buffer.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-20-09
///
///\brief

#include "index_buffer.h"

namespace circe::gl {

IndexBuffer::IndexBuffer() = default;

IndexBuffer::~IndexBuffer() = default;

IndexBuffer &IndexBuffer::operator=(IndexBuffer &&other) noexcept {
  element_count = other.element_count;
  element_type = other.element_type;
  data_type = other.data_type;
  access_ = other.access_;
  this->using_external_memory_ = other.using_external_memory_;
  dm_ = std::move(other.dm_);
  if (using_external_memory_)
    mem_ = std::move(other.mem_);
  else
    mem_ = std::make_unique<DeviceMemory::View>(dm_);
  return *this;
}

u64 IndexBuffer::dataSizeInBytes() const {
  u64 index_count = 0;
  switch (element_type) {
  case GL_TRIANGLES: index_count = element_count * 3;
    break;
  case GL_TRIANGLE_STRIP:
  case GL_TRIANGLE_FAN: index_count = element_count + 2;
    break;
  case GL_LINES: index_count = element_count * 2;
    break;
  case GL_LINE_STRIP: index_count = element_count + 1;
    break;
  case GL_LINE_LOOP: index_count = element_count;
    break;
  default:spdlog::warn("Assuming one index per element in index buffer size.");
    index_count = element_count;
  }
  return index_count * OpenGL::dataSizeInBytes(data_type);
}

void IndexBuffer::draw() {
  static u64 last_element_count = 0;
  static GLuint last_element_type = 0;
  static u32 index_count = 0;
  if (last_element_type != element_type || last_element_count != element_count) {
    last_element_count = element_count;
    last_element_type = element_type;
    switch (element_type) {
    case GL_TRIANGLES: index_count = element_count * 3;
      break;
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN: index_count = element_count + 2;
      break;
    case GL_LINES: index_count = element_count * 2;
      break;
    case GL_LINE_STRIP: index_count = element_count + 1;
      break;
    case GL_LINE_LOOP: index_count = element_count;
      break;
    default:spdlog::warn("Assuming one index per element in index buffer size.");
      index_count = element_count;
    };
  }
  mem_->bind();
  CHECK_GL(glDrawElements(element_type, index_count, data_type,
                          reinterpret_cast<void *>(mem_->offset())));
}

GLuint IndexBuffer::bufferTarget() const {
  return GL_ELEMENT_ARRAY_BUFFER;
}

GLuint IndexBuffer::bufferUsage() const {
  return GL_STATIC_DRAW;
}

std::ostream &operator<<(std::ostream &os, const IndexBuffer &index_buffer) {
  os << "Index Buffer (" << index_buffer.element_count << " elements)(" <<
     OpenGL::primitiveGLName(index_buffer.element_type) << ")(" <<
     OpenGL::TypeToStr(index_buffer.data_type) << ")" << std::endl;
  return os;
}

}
