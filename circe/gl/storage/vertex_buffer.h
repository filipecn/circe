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
///\file vertex_buffer.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-20-09
///
///\brief

#ifndef PONOS_CIRCE_CIRCE_GL_STORAGE_VERTEX_BUFFER_H
#define PONOS_CIRCE_CIRCE_GL_STORAGE_VERTEX_BUFFER_H

#include <circe/gl/storage/buffer_interface.h>
#include <hermes/storage/array_of_structures.h>
#include <circe/gl/storage/vertex_attributes.h>
#include <string>

namespace circe::gl {

/// A Vertex Buffer Object (VBO) is the common term for a normal Buffer Object when
/// it is used as a source for vertex array data.
/// The VBO carries the description of attributes stored in the buffer. This
/// description is then used to pass the correct data to shaders.
///
/// Example:
/// Consider the following interleaved vertex buffer with N+1 vertices
/// px0 py0 pz0 u0 v0 nx0 ny0 nz0 ... pxN pyN pzN uN vN nxN nyN nzN
/// containing position, texture coordinates and normal for each vertex.
/// Data is stored in floats (4 bytes)
/// Note: VertexAttributes must be pushed in order: position, texture coord. and normal.
/// The position attribute has offset 0 * 4, size 3 and type GL_FLOAT
/// The tex coord attribute has offset 3 * 4, size 2 and type GL_FLOAT
/// The normal attribute has offset 5 * 4, size 3 and type GL_FLOAT
class VertexBuffer : public BufferInterface {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  VertexBuffer();
  VertexBuffer(VertexBuffer &&other) noexcept;
  ~VertexBuffer() override;
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  VertexBuffer &operator=(const hermes::AoS &aos);
  VertexBuffer &operator=(VertexBuffer &&other) noexcept;
  // ***********************************************************************
  //                             METHODS
  // ***********************************************************************
  /// \param binding_index new binding index value
  void setBindingIndex(GLuint binding_index);
  [[nodiscard]] GLuint bufferTarget() const override;
  [[nodiscard]] GLuint bufferUsage() const override;
  u64 dataSizeInBytes() const override;
  u64 vertexCount() const { return vertex_count_; }
  /// \tparam T
  /// \param data
  /// \return
  template<typename T>
  VertexBuffer &operator=(const std::vector<T> &data) {
    auto data_type_size = sizeof(T);
    auto data_size = data.size() * data_type_size;
    vertex_count_ = data_size / attributes.stride();
    setData(reinterpret_cast<const void *>(data.data()));
    return *this;
  }
  /// \param n
  void resize(u32 n);
  /// glBindVertexBuffer
  void bind() override;
  /// Note: A vertex array object must be bound before calling this method
  void bindAttributeFormats();
  /// debug
  friend std::ostream &operator<<(std::ostream &os, const VertexBuffer &vb);
  std::string memoryDump(hermes::memory_dumper_options options =
  hermes::memory_dumper_options::type_values |
      hermes::memory_dumper_options::colored_output) const;

  // public fields
  VertexAttributes attributes; //!< attribute description

private:
  u64 vertex_count_{0};
  GLuint binding_index_{0};
};

}

#endif //PONOS_CIRCE_CIRCE_GL_STORAGE_VERTEX_BUFFER_H
