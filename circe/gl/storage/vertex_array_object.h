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
///\file vertex_array_object.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-11-09
///
///\brief

#ifndef PONOS_CIRCE_CIRCE_GL_IO_VERTEX_ARRAY_OBJECT_H
#define PONOS_CIRCE_CIRCE_GL_IO_VERTEX_ARRAY_OBJECT_H

#include <circe/gl/utils/open_gl.h>

namespace circe::gl {

/// A Vertex Array Object (VAO) is an OpenGL Object that stores all the
/// state needed to supply vertex data. It stores the format of the vertex data
/// as well as the Buffer Objects (see below) providing the vertex data arrays.
///
/// Notes:
/// - This class uses RAII. The object is created on construction and destroyed on
/// deletion.
class VertexArrayObject {
public:
  /// Default Constructor (creates the OpenGL VAO object)
  VertexArrayObject();
  /// VAO object cannot be copied.
  VertexArrayObject(const VertexArrayObject &) = delete;
  VertexArrayObject &operator=(const VertexArrayObject &) = delete;
  /// Move constructor
  VertexArrayObject(VertexArrayObject &&other) noexcept;
  ~VertexArrayObject();
  /// Self-assignment operator
  VertexArrayObject &operator=(VertexArrayObject &&other) noexcept;
  /// glBind
  void bind() const;
  /// glDelete
  void destroy();
  /// \return OpenGL object id
  [[nodiscard]] inline GLuint id() const { return vao_object_id_; }
private:
  GLuint vao_object_id_{0};
};

}

#endif //PONOS_CIRCE_CIRCE_GL_IO_VERTEX_ARRAY_OBJECT_H
