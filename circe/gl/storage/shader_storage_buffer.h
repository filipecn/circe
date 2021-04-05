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

#ifndef CIRCE_CIRCE_GL_STORAGE_SHADER_STORAGE_BUFFER_H
#define CIRCE_CIRCE_GL_STORAGE_SHADER_STORAGE_BUFFER_H

#include <circe/gl/storage/buffer_interface.h>
#include <ponos/storage/array_of_structures.h>

namespace circe::gl {

class ShaderStorageBuffer : public BufferInterface {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  ShaderStorageBuffer();
  ShaderStorageBuffer(ShaderStorageBuffer &&other) noexcept;
  ~ShaderStorageBuffer() override;
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  ShaderStorageBuffer &operator=(const ponos::AoS &aos);
  ShaderStorageBuffer &operator=(ShaderStorageBuffer &&other) noexcept;
  // ***********************************************************************
  //                             METHODS
  // ***********************************************************************
  [[nodiscard]] GLuint bufferTarget() const override;
  [[nodiscard]] GLuint bufferUsage() const override;
  u64 dataSizeInBytes() const override;
  void bind() override;
  void setBindingIndex(GLuint binding_index);
  // ***********************************************************************
  //                          PUBLIC FIELDS
  // ***********************************************************************
  ponos::StructDescriptor descriptor;

private:
  u64 struct_count_{0};
  GLuint binding_index_{0};
};

}

#endif //CIRCE_CIRCE_GL_STORAGE_SHADER_STORAGE_BUFFER_H
