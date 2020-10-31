// Created by FilipeCN on 2/21/2018.
/*
 * Copyright (c) 2018 FilipeCN
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

#ifndef CIRCE_SCENE_MESH_H
#define CIRCE_SCENE_MESH_H

#include <circe/gl/io/buffer.h>

namespace circe::gl {

/// Holds a vertex array object containing a combination of a vertex buffer
/// and index buffer.
//class SceneModel {
//public:
//  SceneModel();
//  virtual ~SceneModel();
//  bool set(const ponos::RawMesh &raw_mesh);
//  void bind() const;
//  const GLVertexBuffer &vertexBuffer() const { return vertex_buffer_; }
//  const GLIndexBuffer &indexBuffer() const { return index_buffer_; }
//  void draw();
//  void clear();
//protected:
//  GLuint VAO{0};
//  std::vector<float> vertex_data_;
//  std::vector<uint> index_data_;
//  GLVertexBuffer vertex_buffer_;
//  GLIndexBuffer index_buffer_;
//};

/// Set of buffers that represent a raw mesh for rendering.
class SceneMesh {
public:
  SceneMesh() = default;
  /// \param rm raw mesh
  explicit SceneMesh(const ponos::RawMesh *rm);
  ~SceneMesh();
  bool set(const ponos::RawMesh *rm);
  /// Binds buffers
  void bind();
  void unbind();
  /// \return vertex buffer pointer
  const GLVertexBuffer *vertexBuffer() const;
  /// \return index buffer pointer
  const GLIndexBuffer *indexBuffer() const;
  const ponos::RawMesh *rawMesh() const;

protected:
  GLuint VAO;
  std::vector<float> vertexData_;
  std::vector<uint> indexData_;
  const ponos::RawMesh *mesh_ = nullptr;
  GLVertexBuffer vertexBuffer_;
  GLIndexBuffer indexBuffer_;
};

/// Set of buffers created that represent a raw mesh for rendering. The mesh
/// data can be updated.
class SceneDynamicMesh {
public:
  /// \brief Construct a new Scene Dynamic Mesh object
  SceneDynamicMesh();
  /// \brief Construct a new Scene Dynamic Mesh object
  /// \param vertex_buffer_desc **[in]**
  /// \param index_buffer_desc **[in]**
  SceneDynamicMesh(const BufferDescriptor &vertex_buffer_desc,
                   const BufferDescriptor &index_buffer_desc);
  /// \brief Destroy the Scene Dynamic Mesh object
  ~SceneDynamicMesh();
  /// Binds buffers
  void bind();
  /// Unbinds buffers
  void unbind();
  /// \return vertex buffer pointer
  [[nodiscard]] const GLVertexBuffer *vertexBuffer() const;
  /// \return index buffer pointer
  [[nodiscard]] const GLIndexBuffer *indexBuffer() const;
  /// \param vertex_buffer_data **[in]**
  /// \param vertex_count **[in]**
  /// \param index_buffer_data **[in]**
  /// \param mesh_element_count **[in]**
  void update(float *vertex_buffer_data, size_t vertex_count,
              uint *index_buffer_data, size_t mesh_element_count);
  /// \brief set descriptions
  /// \param vertex_buffer_desc **[in]**
  /// \param index_buffer_desc **[in]**
  void setDescription(const BufferDescriptor &vertex_buffer_desc,
                      const BufferDescriptor &index_buffer_desc);

private:
  GLuint VAO_;
  GLVertexBuffer vertex_buffer_;
  GLIndexBuffer index_buffer_;
  BufferDescriptor vertex_buffer_descriptor_;
  BufferDescriptor index_buffer_descriptor_;
};

using SceneMeshSPtr = std::shared_ptr<SceneMesh>;

template<typename... TArg> SceneMeshSPtr createSceneMeshPtr(TArg &&... Args) {
  return std::make_shared<SceneMesh>(std::forward<TArg>(Args)...);
}

} // namespace circe

#endif // CIRCE_SCENE_MESH_H
