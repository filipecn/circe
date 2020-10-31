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
///\file uniform_buffer.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-21-09
///
///\brief

#include "uniform_buffer.h"

namespace circe::gl {

UniformBuffer::UniformBuffer() = default;

UniformBuffer::~UniformBuffer() = default;

GLuint UniformBuffer::bufferTarget() const {
  return GL_UNIFORM_BUFFER;
}

GLuint UniformBuffer::bufferUsage() const {
  return GL_DYNAMIC_DRAW;
}

u64 UniformBuffer::dataSizeInBytes() const {
  return total_size_;
}

void UniformBuffer::push(const Program &program) {
  needs_update_ = true;
  const auto &uniforms = program.uniforms();
  for (const auto &ub : program.uniformBlocks()) {
    UniformBlockData ubd(*this);
    ubd.name_ = ub.name;
    ubd.buffer_binding_ = ub.buffer_binding;
    ubd.size_ = ub.size_in_bytes;
    for (const auto &i : ub.variable_indices)
      ubd.variables_.emplace_back(uniforms[i]);
    ubd.offset_ = total_size_;
    uniform_blocks_.emplace_back(ubd);
    total_size_ += ubd.size_;
  }
}

UniformBuffer::UniformBlockData &UniformBuffer::operator[](const std::string &block_name) {
  for (auto &ubd : uniform_blocks_)
    if (ubd.name_ == block_name)
      return ubd;
  spdlog::error("Invalid Uniform Block Name (returning block 0 instead)");
  return uniform_blocks_[0];
}

void UniformBuffer::setData(const void *data, u64 offset, u64 size) {
  if (!mem_) {
    // allocate if necessary
    allocate(GL_DYNAMIC_DRAW);
  }
  // if needs update, we need to connect binding points!
  if (needs_update_) {
    mem_->bind();
    for (const auto &ub : uniform_blocks_)
      glBindBufferRange(GL_UNIFORM_BUFFER, ub.buffer_binding_, mem_->bufferId(),
                        ub.offset_, ub.size_);
    needs_update_ = false;
  }

  auto *m = mem_->mapped(offset, size, GL_MAP_WRITE_BIT);
  std::memcpy(m, data, dataSizeInBytes());
  mem_->unmap();
}

std::ostream &operator<<(std::ostream &os, const UniformBuffer::UniformBlockData &ubd) {
  os << "Buffer Uniform Data (offset " <<
     ubd.offset_ << ") (" << ubd.size_ << " bytes) (binding "
     << ubd.buffer_binding_ << ") " << ubd.name_ << "\n";
#define PRINT_TO_STR(A) \
os << "\t\t" << #A << ": " << (A) << std::endl;
  std::string glsl_type;
  for (const auto &u : ubd.variables_) {
    os << "\tVariable (Uniform #" << u.index << "):" << std::endl;
    PRINT_TO_STR(u.index)
    PRINT_TO_STR(u.name)
    PRINT_TO_STR(u.offset)
    os << "\t\tu.type: " << OpenGL::TypeToStr(u.type, &glsl_type);
    os << " (" << glsl_type << ")\n";
    PRINT_TO_STR(u.block_index)
    PRINT_TO_STR(u.array_size)
    PRINT_TO_STR(u.array_stride)
    PRINT_TO_STR(u.matrix_stride)
    PRINT_TO_STR(u.is_row_major)
    PRINT_TO_STR(u.location)
  }
  return os;
#undef PRINT_TO_STR
}

std::ostream &operator<<(std::ostream &os, UniformBuffer &uniform_buffer) {
  os << "Uniform Buffer (" << uniform_buffer.uniform_blocks_.size() << " blocks) ("
     << uniform_buffer.total_size_ << " bytes)\n";
  for (u64 i = 0; i < uniform_buffer.uniform_blocks_.size(); ++i) {
    const auto &ub = uniform_buffer.uniform_blocks_[i];
    os << "Block #" << i << ":";
    os << ub << std::endl;
  }
  os << "raw content: ";
  if (uniform_buffer.mem_) {
    auto data = uniform_buffer.mem_->rawData();
    for (auto d : data)
      os << " " << (u32) d;
    os << std::endl;
  } else
    os << "(not allocated)\n";
  return os;
}

}