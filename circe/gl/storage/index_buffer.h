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
///\file index_buffer.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-20-09
///
///\brief

#ifndef PONOS_CIRCE_CIRCE_GL_STORAGE_INDEX_BUFFER_H
#define PONOS_CIRCE_CIRCE_GL_STORAGE_INDEX_BUFFER_H

#include <circe/gl/storage/buffer_interface.h>

namespace circe::gl {

/// A Element Buffer Object (EBO) stores the topology of a mesh. A set of
/// indices assigned by a geometric primitive.
class IndexBuffer : public BufferInterface {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  IndexBuffer();
  ~IndexBuffer() override;
  // ***********************************************************************
  //                             METHODS
  // ***********************************************************************
  [[nodiscard]] GLuint bufferTarget() const override;
  [[nodiscard]] GLuint bufferUsage() const override;
  [[nodiscard]] u64 dataSizeInBytes() const override;
  /// glDrawElements
  void draw();
  // ***********************************************************************
  //                            OPERATORS
  // ***********************************************************************
  IndexBuffer &operator=(IndexBuffer &&other) noexcept;
  /// \tparam T
  /// \param data
  /// \return
  template<typename T, typename std::enable_if_t<
      std::is_same_v<i32, T> || std::is_same_v<i16, T> || std::is_same_v<i8, T> ||
          std::is_same_v<u32, T> || std::is_same_v<u16, T> || std::is_same_v<u8, T>> * = nullptr>
  IndexBuffer &operator=(const std::vector<T> &data) {
    auto data_type_size = sizeof(T);
    switch (data_type_size) {
    case 4: data_type = GL_UNSIGNED_INT;
      break;
    case 2: data_type = GL_UNSIGNED_SHORT;
      break;
    default:data_type = GL_UNSIGNED_BYTE;
    }
    switch (element_type) {
    case GL_TRIANGLES: element_count = data.size() / 3;
      break;
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN: element_count = data.size() - 2;
      break;
    case GL_LINES:element_count = data.size() / 2;
      break;
    case GL_LINE_STRIP: element_count = data.size() - 1;
      break;
    case GL_LINE_LOOP: element_count = data.size();
      break;
    default:spdlog::warn("Element type considered GL_POINTS on index buffer assignment.");
      element_count = data.size();
    }
    setData(reinterpret_cast<const void *>(data.data()));
    return *this;
  }
  // ***********************************************************************
  //                          PUBLIC FIELDS
  // ***********************************************************************
  u64 element_count{0};
  GLuint element_type{GL_TRIANGLES};
  GLuint data_type{GL_UNSIGNED_INT};
};

std::ostream &operator<<(std::ostream &os, const IndexBuffer &index_buffer);

}

#endif //PONOS_CIRCE_CIRCE_GL_STORAGE_INDEX_BUFFER_H
