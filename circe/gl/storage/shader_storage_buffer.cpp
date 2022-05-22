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
///\file shader_storage_buffer.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-02-10
///
///\brief

#include <circe/gl/storage/shader_storage_buffer.h>

namespace circe::gl {

ShaderStorageBuffer::ShaderStorageBuffer() = default;

ShaderStorageBuffer::ShaderStorageBuffer(ShaderStorageBuffer &&other) noexcept {
  descriptor = other.descriptor;
  struct_count_ = other.struct_count_;
  binding_index_ = other.binding_index_;
  access_ = other.access_;
  dm_ = std::move(other.dm_);
  this->using_external_memory_ = other.using_external_memory_;
  if (using_external_memory_)
    mem_ = std::move(other.mem_);
  else
    mem_ = std::make_unique<DeviceMemory::View>(dm_);
}

ShaderStorageBuffer::~ShaderStorageBuffer() = default;

ShaderStorageBuffer &ShaderStorageBuffer::operator=(const hermes::AoS &aos) {
  descriptor = aos.structDescriptor();
  struct_count_ = aos.size();
  setData(reinterpret_cast<const void *>(aos.data()));
  return *this;
}

ShaderStorageBuffer &ShaderStorageBuffer::operator=(ShaderStorageBuffer &&other) noexcept {
  descriptor = other.descriptor;
  struct_count_ = other.struct_count_;
  binding_index_ = other.binding_index_;
  access_ = other.access_;
  dm_ = std::move(other.dm_);
  this->using_external_memory_ = other.using_external_memory_;
  if (using_external_memory_)
    mem_ = std::move(other.mem_);
  else
    mem_ = std::make_unique<DeviceMemory::View>(dm_);
  return *this;
}

GLuint ShaderStorageBuffer::bufferTarget() const {
  return GL_SHADER_STORAGE_BUFFER;
}

GLuint ShaderStorageBuffer::bufferUsage() const {
  return GL_DYNAMIC_COPY;
}

u64 ShaderStorageBuffer::dataSizeInBytes() const {
  return descriptor.sizeInBytes() * struct_count_;
}

void ShaderStorageBuffer::bind() {
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_index_, mem_->deviceMemory().id());
  CHECK_GL_ERRORS;
}

void ShaderStorageBuffer::setBindingIndex(GLuint binding_index) {
  binding_index_ = binding_index;
}
ShaderStorageBuffer ShaderStorageBuffer::fromVertexBuffer(VertexBuffer &vertex_buffer) {
  ShaderStorageBuffer vertex_ssbo;
  vertex_ssbo.descriptor = vertex_buffer.structDescriptor();
  vertex_ssbo.struct_count_ = vertex_buffer.vertexCount();
  vertex_ssbo.attachMemory(vertex_buffer.memory(), 0);
  return vertex_ssbo;
}

ShaderStorageBuffer ShaderStorageBuffer::fromIndexBuffer(IndexBuffer &index_buffer) {
  ShaderStorageBuffer index_ssbo;
  index_ssbo.descriptor.pushField<i32>("index");
  index_ssbo.struct_count_ = index_buffer.element_count * OpenGL::primitiveSize(index_buffer.element_type);
  index_ssbo.attachMemory(index_buffer.memory(), 0);
  return index_ssbo;
}

}