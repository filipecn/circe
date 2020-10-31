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

#include <circe/gl/scene/scene_mesh.h>

namespace circe::gl {

//SceneModel::SceneModel() = default;
//
//SceneModel::~SceneModel() { clear(); }
//
//bool SceneModel::set(const ponos::RawMesh &raw_mesh) {
//  clear();
//  setup_buffer_data_from_mesh(raw_mesh, vertex_data_, index_data_);
//  BufferDescriptor ver, ind;
//  create_buffer_description_from_mesh(raw_mesh, ver, ind);
//  glGenVertexArrays(1, &VAO);
//  glBindVertexArray(VAO);
//  vertex_buffer_.set(&vertex_data_[0], ver);
//  index_buffer_.set(&index_data_[0], ind);
//  glBindBuffer(GL_ARRAY_BUFFER, 0);
//  vertex_buffer_.bind();
//  index_buffer_.bind();
//  glBindVertexArray(0);
//  CHECK_GL_ERRORS;
//  return true;
//}

//void SceneModel::bind() const {
//  glBindVertexArray(VAO);
//}

//void SceneModel::draw() {
//  glBindVertexArray(VAO);
//  glEnable(GL_DEPTH_TEST);
//  glDrawElements(index_buffer_.bufferDescriptor.element_type,
//                 index_buffer_.bufferDescriptor.element_count *
//                     index_buffer_.bufferDescriptor.element_size,
//                 GL_UNSIGNED_INT, 0);
//}

//void SceneModel::clear() {
//  glBindVertexArray(0);
//  if (VAO)
//    glDeleteVertexArrays(1, &VAO);
//  vertex_data_.clear();
//  index_data_.clear();
//TODO :
//clear buffer
//too
//}

SceneMesh::SceneMesh(const ponos::RawMesh *rm) : mesh_(rm) {
  set(rm);
}

SceneMesh::~SceneMesh() {
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &VAO);
}

bool SceneMesh::set(const ponos::RawMesh *rm) {
  mesh_ = rm;
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &VAO);
  vertexData_.clear();
  indexData_.clear();
  setup_buffer_data_from_mesh(*rm, vertexData_, indexData_);
  BufferDescriptor ver, ind;
  create_buffer_description_from_mesh(*mesh_, ver, ind);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  vertexBuffer_.set(&vertexData_[0], ver);
  indexBuffer_.set(&indexData_[0], ind);
  // vertexBuffer_.set(&mesh_->interleavedData[0], ver);
  // indexBuffer_.set(&mesh_->positionsIndices[0], ind);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  CHECK_GL_ERRORS;
  return true;
}

void SceneMesh::bind() {
  glBindVertexArray(VAO);
  vertexBuffer_.bind();
  indexBuffer_.bind();
}

const GLVertexBuffer *SceneMesh::vertexBuffer() const { return &vertexBuffer_; }

const GLIndexBuffer *SceneMesh::indexBuffer() const { return &indexBuffer_; }

const ponos::RawMesh *SceneMesh::rawMesh() const { return mesh_; }

void SceneMesh::unbind() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

SceneDynamicMesh::SceneDynamicMesh() {
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);
}

SceneDynamicMesh::~SceneDynamicMesh() {
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &VAO_);
}

SceneDynamicMesh::SceneDynamicMesh(const BufferDescriptor &vertex_buffer_desc,
                                   const BufferDescriptor &index_buffer_desc)
    : SceneDynamicMesh() {
  setDescription(vertex_buffer_desc, index_buffer_desc);
}

void SceneDynamicMesh::update(float *vertex_buffer_data, size_t vertex_count,
                              uint *index_buffer_data,
                              size_t mesh_element_count) {
  vertex_buffer_descriptor_.element_count = vertex_count;
  index_buffer_descriptor_.element_count = mesh_element_count;
  glBindVertexArray(VAO_);
  vertex_buffer_.set(vertex_buffer_data, vertex_buffer_descriptor_);
  index_buffer_.set(index_buffer_data, index_buffer_descriptor_);
  CHECK_GL_ERRORS;
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void SceneDynamicMesh::setDescription(
    const BufferDescriptor &vertex_buffer_descriptor,
    const BufferDescriptor &index_buffer_descriptor) {
  vertex_buffer_descriptor_ = vertex_buffer_descriptor;
  index_buffer_descriptor_ = index_buffer_descriptor;
}

void SceneDynamicMesh::bind() {
  glBindVertexArray(VAO_);
  vertex_buffer_.bind();
  index_buffer_.bind();
}

const GLVertexBuffer *SceneDynamicMesh::vertexBuffer() const {
  return &vertex_buffer_;
}

const GLIndexBuffer *SceneDynamicMesh::indexBuffer() const {
  return &index_buffer_;
}

void SceneDynamicMesh::unbind() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

} // namespace circe