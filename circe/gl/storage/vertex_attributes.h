//
// Created by filipecn on 26/11/2021.
//


/// Copyright (c) 2021, FilipeCN.
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
///\file vertex_attributes.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-11-26
///
///\brief

/// pbrt code is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
/// The pbrt source code is licensed under the Apache License, Version 2.0.
/// SPDX: Apache-2.0

#ifndef CIRCE_CIRCE_GL_STORAGE_VERTEX_ATTRIBUTES_H
#define CIRCE_CIRCE_GL_STORAGE_VERTEX_ATTRIBUTES_H

#include <circe/gl/utils/open_gl.h>
#include <string>

namespace circe::gl {

class VertexAttributes {
  friend class VertexBuffer;
public:
  /// Attribute description
  /// Example:
  /// Consider a position attribute (hermes::point3, f32 p[3], ...). Its description
  /// would be
  ///  - name = "position" (optional)
  ///  - size = 3 (x y z)
  ///  - type = GL_FLOAT
  struct Attribute {
    std::string name; //!< attribute name (optional)
    u64 size{0};    //!< attribute number of components
    GLenum type{0}; //!< attribute data type (GL_FLOAT,...)
    GLboolean normalized{GL_FALSE}; //!< Specifies whether fixed-point data values
    //!< should be normalized (GL_TRUE) or converted directly
    //!< as fixed-point values (GL_FALSE)
    u32 divisor{0};
    u32 location{0};
    size_t componentSize() const {
      size_t component_size = 1;
      if (size % 3 == 0)
        component_size = 3;
      else if (size % 4 == 0)
        component_size = 4;
      else if (size % 2 == 0)
        component_size = 2;
      return component_size;
    }
    [[nodiscard]] size_t rows() const {
      return size / componentSize();
    }

  };
  explicit VertexAttributes();
  /// Param constructor
  /// \param attributes
  explicit VertexAttributes(const std::vector<Attribute> &attributes);
  ~VertexAttributes();
  void clear();
  bool contains(const std::string &attribute_name) const;
  /// \param attributes
  void push(const std::vector<Attribute> &attributes);
  /// Append attribute to vertex format description. Push order is important, as
  /// attribute offsets are computed as attributes are appended.
  /// \param component_count
  /// \param name attribute name
  /// \param data_type
  /// \param normalized
  /// \return attribute id
  /// \param location
  /// \return
  u64 push(u64 component_count, const std::string &name = "", GLenum data_type = GL_FLOAT,
           GLboolean normalized = GL_FALSE, int location = -1);
  ///
  /// \param attribute
  /// \return
  u64 push(const Attribute &attribute);
  ///
  /// Push attribute using a hermes type
  /// \tparam T accepts only hermes::MathElement derived objects (point, vector, matrix,...)
  /// \param name attribute name (optional)
  /// \param location (default = -1) if -1, then location is incremented from last pushed attribute
  /// \return
  template<typename T,
      typename std::enable_if_t<
          std::is_base_of_v<hermes::MathElement<f32, 2u>, T> ||
              std::is_base_of_v<hermes::MathElement<f32, 3u>, T> ||
              std::is_base_of_v<hermes::MathElement<f32, 4u>, T> ||
              std::is_base_of_v<hermes::MathElement<f32, 9u>, T> ||
              std::is_base_of_v<hermes::MathElement<f32, 16u>, T>> * = nullptr>
  u64 push(const std::string &name = "", i32 location = -1) {
    return push(T::componentCount(), name, OpenGL::dataTypeEnum<decltype(T::numeric_data)>());
  }
  /// \return data size in bytes of a single vertex
  inline u64 stride() const {
    return stride_;
  }
  inline u64 attributeOffset(u64 attribute_index) const { return offsets_[attribute_index]; }
  inline const std::vector<Attribute> &attributes() const { return attributes_; }
  inline u64 attributeIndex(const std::string &attribute_name) const {
    auto it = attribute_name_id_map_.find(attribute_name);
    return it->second;
  }

  void setAttributeLocation(u64 id, GLint location);

  ///
  /// \param binding_index
  void bindFormats(GLuint binding_index) const;

  friend std::ostream &operator<<(std::ostream &os, const VertexAttributes &attr);

private:
  void updateOffsets();

  std::unordered_map<std::string, u64> attribute_name_id_map_;
  std::vector<Attribute> attributes_;
  std::vector<u64> offsets_;  //!< attribute data offset (in bytes)
  u64 stride_{0};
};

}

#endif //CIRCE_CIRCE_GL_STORAGE_VERTEX_ATTRIBUTES_H
