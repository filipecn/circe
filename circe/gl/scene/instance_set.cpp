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

//
// Created by FilipeCN on 2/19/2018.
//

#include <circe/gl/scene/instance_set.h>

#include <utility>

namespace circe::gl {

InstanceSet::InstanceSet() = default;

InstanceSet::InstanceSet(SceneMesh *m, const ShaderProgram &s, size_t n)
    : InstanceSet() {
  shader_ = s;
  if (n)
    resize(n);
}

InstanceSet::~InstanceSet() {
  for (auto &buffer : buffers_)
    delete buffer;
}

uint InstanceSet::add(BufferDescriptor d) {
  d.element_count = count_;
  size_t b = buffers_.size();
  size_t bs = d.element_count * d.element_size;
  switch (d.data_type) {
  case GL_UNSIGNED_INT: {
    buffers_.emplace_back(new GLBuffer<uint>(d));
    dataU_.emplace_back(std::vector<uint>());
    buffers_indices_.emplace_back(dataU_.size() - 1);
    if (count_)
      dataU_[dataU_.size() - 1].resize(bs);
    break;
  }
  case GL_UNSIGNED_BYTE: {
    buffers_.emplace_back(new GLBuffer<uint>(d));
    dataC_.emplace_back(std::vector<uchar>());
    buffers_indices_.emplace_back(dataC_.size() - 1);
    if (count_)
      dataC_[dataC_.size() - 1].resize(bs);
    break;
  }
  default: {
    buffers_.emplace_back(new GLBuffer<float>(d));
    dataF_.emplace_back(std::vector<float>());
    buffers_indices_.emplace_back(dataF_.size() - 1);
    if (count_)
      dataF_[dataF_.size() - 1].resize(bs);
  }
  }
  //  for (auto &attribute : d.attributes)
  //    shader_.addVertexAttribute(attribute.first.c_str());
  return b;
}

void InstanceSet::resize(uint n) {
  count_ = n;
  for (uint i = 0; i < buffers_.size(); i++) {
    buffers_[i]->bufferDescriptor.element_count = n;
    switch (buffers_[i]->bufferDescriptor.data_type) {
    case GL_UNSIGNED_INT:
      dataU_[buffers_indices_[i]].resize(
          buffers_[i]->bufferDescriptor.element_count *
              buffers_[i]->bufferDescriptor.element_size);
      dynamic_cast<GLBuffer<uint> *>(buffers_[i])
          ->set(&dataU_[buffers_indices_[i]][0], buffers_[i]->bufferDescriptor);
      break;
    case GL_UNSIGNED_BYTE:
      dataC_[buffers_indices_[i]].resize(
          buffers_[i]->bufferDescriptor.element_count *
              buffers_[i]->bufferDescriptor.element_size);
      dynamic_cast<GLBuffer<uchar> *>(buffers_[i])
          ->set(&dataC_[buffers_indices_[i]][0], buffers_[i]->bufferDescriptor);
      break;
    default:
      dataF_[buffers_indices_[i]].resize(
          buffers_[i]->bufferDescriptor.element_count *
              buffers_[i]->bufferDescriptor.element_size);
      dynamic_cast<GLBuffer<float> *>(buffers_[i])
          ->set(&dataF_[buffers_indices_[i]][0], buffers_[i]->bufferDescriptor);
    }
  }
  data_changed_.resize(n, false);
}

float *InstanceSet::instanceF(uint b, uint i) {
  if (i >= count_)
    resize(i + 1);
  data_changed_[b] = true;
  return &dataF_[buffers_indices_[b]]
  [i * buffers_[b]->bufferDescriptor.element_size];
}

uint *InstanceSet::instanceU(uint b, uint i) {
  if (i >= count_)
    resize(i + 1);
  data_changed_[b] = true;
  return &dataU_[buffers_indices_[b]]
  [i * buffers_[b]->bufferDescriptor.element_size];
}

void InstanceSet::bind(uint b) {
  if (data_changed_[b]) {
    switch (buffers_[b]->bufferDescriptor.data_type) {
    case GL_UNSIGNED_INT:
      dynamic_cast<GLBuffer<uint> *>(buffers_[b])
          ->set(&dataU_[buffers_indices_[b]][0]);
      break;
    case GL_UNSIGNED_BYTE:
      dynamic_cast<GLBuffer<uchar> *>(buffers_[b])
          ->set(&dataC_[buffers_indices_[b]][0]);
      break;
    default:
      dynamic_cast<GLBuffer<float> *>(buffers_[b])
          ->set(&dataF_[buffers_indices_[b]][0]);
    }
    data_changed_[b] = false;
  }
  buffers_[b]->bind();
}

void InstanceSet::draw(const CameraInterface *camera,
                       ponos::Transform transform) {
  UNUSED_VARIABLE(transform);
  if (!base_mesh_ || !count_)
    return;
  // bind buffers and locate attributes
  shader_.begin();
  base_mesh_->bind();
  base_mesh_->vertexBuffer()->locateAttributes(shader_);
  for (size_t i = 0; i < buffers_.size(); i++) {
    bind(i);
    buffers_[i]->locateAttributes(shader_, 1);
  }
  shader_.setUniform("model_view_matrix",
                     ponos::transpose(camera->getViewTransform().matrix()));
  shader_.setUniform(
      "projection_matrix",
      ponos::transpose(camera->getProjectionTransform().matrix()));
  CHECK_GL_ERRORS;
  //  shader_.setUniform("mvp",
  //  ponos::transpose((camera->getProjectionTransform() *
  //      camera->getViewTransform() * camera->getModelTransform()).matrix()));
  glDrawElementsInstanced(
      base_mesh_->indexBuffer()->bufferDescriptor.element_type,
      base_mesh_->indexBuffer()->bufferDescriptor.element_size *
          base_mesh_->indexBuffer()->bufferDescriptor.element_count,
      base_mesh_->indexBuffer()->bufferDescriptor.data_type, 0, count_);
  CHECK_GL_ERRORS;
  shader_.end();
  base_mesh_->unbind();
}

} // namespace circe
