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
/// Note: Attributes must be pushed in order: position, texture coord. and normal.
/// The position attribute has offset 0 * 4, size 3 and type GL_FLOAT
/// The tex coord attribute has offset 3 * 4, size 2 and type GL_FLOAT
/// The normal attribute has offset 5 * 4, size 3 and type GL_FLOAT
class VertexBuffer : public BufferInterface {
public:
  class Attributes {
    friend class VertexBuffer;
  public:
    /// Attribute description
    /// Example:
    /// Consider a position attribute (ponos::point3, f32 p[3], ...). Its description
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
    };
    explicit Attributes();
    /// Param constructor
    /// \param attributes
    explicit Attributes(const std::vector<Attribute> &attributes);
    ~Attributes();
    void clear();
    /// \param attributes
    void push(const std::vector<Attribute> &attributes);
    /// Append attribute to vertex format description. Push order is important, as
    /// attribute offsets are computed as attributes are appended.
    /// \param component_count
    /// \param name attribute name
    /// \param data_type
    /// \param normalized
    /// \return attribute id
    u64 push(u64 component_count, const std::string &name = "", GLenum data_type = GL_FLOAT,
             GLboolean normalized = GL_FALSE);
    /// Push attribute using a ponos type
    /// \tparam T accepts only ponos::MathElement derived objects (point, vector, matrix,...)
    /// \param name attribute name (optional)
    /// \return attribute id
    template<typename T,
        typename std::enable_if_t<
            std::is_base_of_v<ponos::MathElement<f32, 2u>, T> ||
                std::is_base_of_v<ponos::MathElement<f32, 3u>, T> ||
                std::is_base_of_v<ponos::MathElement<f32, 4u>, T> ||
                std::is_base_of_v<ponos::MathElement<f32, 9u>, T> ||
                std::is_base_of_v<ponos::MathElement<f32, 16u>, T>> * = nullptr>
    u64 push(const std::string &name = "") {
      return push(T::componentCount(), name, OpenGL::dataTypeEnum<decltype(T::numeric_data)>());
    }
    /// \return data size in bytes of a single vertex
    inline u64 stride() const {
      return stride_;
    }
    inline u64 attributeOffset(u64 attribute_index) const { return offsets_[attribute_index]; }
    inline const std::vector<Attribute> &attributes() const { return attributes_; }
  private:
    void updateOffsets();

    std::unordered_map<std::string, u64> attribute_name_id_map_;
    std::vector<Attribute> attributes_;
    std::vector<u64> offsets_;  //!< attribute data offset (in bytes)
    u64 stride_{0};
  };
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  VertexBuffer();
  VertexBuffer(VertexBuffer &&other) noexcept;
  ~VertexBuffer() override;
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  VertexBuffer &operator=(const ponos::AoS &aos);
  VertexBuffer &operator=(VertexBuffer &&other) noexcept ;
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
  /// glBindVertexBuffer
  void bind() override;
  /// Note: A vertex array object must be bound before calling this method
  void bindAttributeFormats();
  /// Access a single attribute of an element
  /// Note: The buffer MUST be previously mapped
  /// \tparam T attribute data type
  /// \param attribute_name
  /// \param element_id
  /// \return reference to attribute value
  template<typename T>
  T &at(const std::string &attribute_name, u64 element_id) {
    static T dummy{};
    if (attributes.attributes_.empty()) {
      return dummy;
    }
    auto it = attributes.attribute_name_id_map_.find(attribute_name);
    if (it == attributes.attribute_name_id_map_.end())
      return dummy;
    auto element_address = element_id * attributes.stride();
    auto attribute_id = it->second;
    auto attribute_address = element_address + attributes.attributeOffset(attribute_id);
    return static_cast<T &>(reinterpret_cast<char *>(mem_->mapped(access_)) + attribute_address);
  }
  /// debug
  friend std::ostream &operator<<(std::ostream &os, const VertexBuffer &vb);

  // public fields
  Attributes attributes; //!< attribute description

private:
  u64 vertex_count_{0};
  GLuint binding_index_{0};
};

}

#endif //PONOS_CIRCE_CIRCE_GL_STORAGE_VERTEX_BUFFER_H
