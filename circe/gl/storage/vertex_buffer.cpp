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
///\file vertex_buffer.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-20-09
///
///\brief

#include "vertex_buffer.h"

namespace circe::gl {

VertexBuffer::Attributes::Attributes() = default;

VertexBuffer::Attributes::Attributes(const std::vector<Attribute> &attributes) {
  push(attributes);
}

VertexBuffer::Attributes::~Attributes() = default;

void VertexBuffer::Attributes::clear() {
  attributes_.clear();
  attribute_name_id_map_.clear();
  offsets_.clear();
  stride_ = 0;
}

void VertexBuffer::Attributes::push(const std::vector<Attribute> &attributes) {
  attributes_ = attributes;
  updateOffsets();
}

u64 VertexBuffer::Attributes::push(u64 component_count,
                                   const std::string &name,
                                   GLenum data_type,
                                   GLboolean normalized) {
  u64 attribute_id = attributes_.size();
  std::string attribute_name = name.empty() ? std::to_string(attribute_id) : name;
  Attribute attr{
      attribute_name,
      component_count,
      data_type,
      normalized
  };
  attributes_.emplace_back(attr);
  attribute_name_id_map_[attribute_name] = attribute_id;
  offsets_.emplace_back(stride_);
  stride_ += component_count * OpenGL::dataSizeInBytes(data_type);
  return attribute_id;
}

void VertexBuffer::Attributes::updateOffsets() {
  if (attributes_.empty()) {
    offsets_.clear();
    return;
  }
  offsets_.resize(attributes_.size());
  u64 offset = 0;
  for (const auto &a : attributes_) {
    offsets_.emplace_back(offset);
    offset += a.size * OpenGL::dataSizeInBytes(a.type);
  }
}

VertexBuffer::VertexBuffer() = default;

VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept {
  attributes = other.attributes;
  vertex_count_ = other.vertex_count_;
  binding_index_ = other.binding_index_;
  access_ = other.access_;
  dm_ = std::move(other.dm_);
  this->using_external_memory_ = other.using_external_memory_;
  if (using_external_memory_)
    mem_ = std::move(other.mem_);
  else
    mem_ = std::make_unique<DeviceMemory::View>(dm_);
}

VertexBuffer::~VertexBuffer() = default;

VertexBuffer &VertexBuffer::operator=(const ponos::AoS &aos) {
  // clear existent data/attributes
  attributes.clear();
  // push new attributes
  for (const auto &field : aos.fields())
    attributes.push(field.component_count, field.name, OpenGL::dataTypeEnum(field.type));
  vertex_count_ = aos.size();
  setData(reinterpret_cast<const void *>(aos.data()));
  return *this;
}

VertexBuffer &VertexBuffer::operator=(VertexBuffer &&other) noexcept {
  attributes = other.attributes;
  vertex_count_ = other.vertex_count_;
  binding_index_ = other.binding_index_;
  access_ = other.access_;
  dm_ = std::move(other.dm_);
  this->using_external_memory_ = other.using_external_memory_;
  if (using_external_memory_)
    mem_ = std::move(other.mem_);
  else
    mem_ = std::make_unique<DeviceMemory::View>(dm_);
  return *this;
}

void VertexBuffer::setBindingIndex(GLuint binding_index) {
  binding_index_ = binding_index;
}

GLuint VertexBuffer::bufferTarget() const {
  return GL_ARRAY_BUFFER;
}

GLuint VertexBuffer::bufferUsage() const {
  return GL_STATIC_DRAW;
}

u64 VertexBuffer::dataSizeInBytes() const {
  return vertex_count_ * attributes.stride();
}

void VertexBuffer::bind() {
  CHECK_GL(glBindVertexBuffer(binding_index_, mem_->deviceMemory().id(),
                              mem_->offset(), attributes.stride()));
}

void VertexBuffer::bindAttributeFormats() {
  bind();
  for (u64 i = 0; i < attributes.attributes_.size(); ++i) {
    glEnableVertexAttribArray(i);
    // specify vertex attribute format
    glVertexAttribFormat(i, attributes.attributes_[i].size,
                         attributes.attributes_[i].type, false, attributes.offsets_[i]);
    // set the details of a single attribute
    glVertexAttribBinding(i, binding_index_);
  }
}

std::ostream &operator<<(std::ostream &os, const VertexBuffer &vb) {
  const auto &attr = vb.attributes.attributes();
  os << "Vertex Buffer (" << vb.vertex_count_ << " vertices)(" << attr.size()
     << " attributes)(stride = " << vb.attributes.stride() << ")\n";
  for (u64 i = 0; i < attr.size(); ++i) {
    os << "\tAttribute[" << i << "] with offset " <<
       vb.attributes.attributeOffset(i) << "\n";
    os << "\t\tname: " << attr[i].name << std::endl;
    os << "\t\tsize: " << attr[i].size << std::endl;
    os << "\t\ttype: " << OpenGL::TypeToStr(attr[i].type) << std::endl;
  }
  return os;
}

}
