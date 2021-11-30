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
///\file vertex_attributes.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-11-26
///
///\brief

/// pbrt code is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
/// The pbrt source code is licensed under the Apache License, Version 2.0.
/// SPDX: Apache-2.0

#include <circe/gl/storage/vertex_attributes.h>

namespace circe::gl {

VertexAttributes::VertexAttributes() = default;

VertexAttributes::VertexAttributes(const std::vector<Attribute> &attributes) {
  push(attributes);
}

VertexAttributes::~VertexAttributes() = default;

void VertexAttributes::clear() {
  attributes_.clear();
  attribute_name_id_map_.clear();
  offsets_.clear();
  stride_ = 0;
}

bool VertexAttributes::contains(const std::string &attribute_name) const {
  return attribute_name_id_map_.find(attribute_name) != attribute_name_id_map_.end();
}

void VertexAttributes::push(const std::vector<Attribute> &attributes) {
  attributes_ = attributes;
  updateOffsets();
}

u64 VertexAttributes::push(u64 component_count,
                           const std::string &name,
                           GLenum data_type,
                           GLboolean normalized, int location) {
  u64 attribute_id = attributes_.size();
  std::string attribute_name = name.empty() ? std::to_string(attribute_id) : name;

  // if location is -1 then it is last location + 1
  if (location < 0) {
    location = attributes_.empty() ? 0 : attributes_.back().location + attributes_.back().rows();
  }

  Attribute attr{
      attribute_name,
      component_count,
      data_type,
      normalized,
      0,
      static_cast<u32>(location)
  };
  attributes_.emplace_back(attr);
  attribute_name_id_map_[attribute_name] = attribute_id;
  offsets_.emplace_back(stride_);
  stride_ += component_count * OpenGL::dataSizeInBytes(data_type);
  return attribute_id;
}

u64 VertexAttributes::push(const VertexAttributes::Attribute &attribute) {
  u64 attribute_id = attributes_.size();
  attributes_.emplace_back(attribute);
  attribute_name_id_map_[attribute.name] = attribute_id;
  offsets_.emplace_back(stride_);
  stride_ += attribute.size * OpenGL::dataSizeInBytes(attribute.type);
  return attribute_id;
}

void VertexAttributes::updateOffsets() {
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

void VertexAttributes::bindFormats(GLuint binding_index) const {
  for (size_t i = 0; i < attributes_.size(); ++i) {
    const auto &attribute = attributes_[i];
    auto component_size = attribute.componentSize();
    for (size_t j = 0; j < attribute.rows(); ++j) {
      // compute attribute index based on the slot component index
      int attribute_index = attribute.location + j;
      int offset = offsets_[i] + component_size * j * 4;
      glEnableVertexAttribArray(attribute_index);
      glVertexAttribFormat(attribute_index, component_size,
                           attribute.type, false, offset);
      glVertexAttribDivisor(attribute_index, attribute.divisor);
      glVertexAttribBinding(attribute_index, binding_index);
    }
  }
  glVertexBindingDivisor(binding_index, attributes_[0].divisor);
  CHECK_GL_ERRORS
}

std::ostream &operator<<(std::ostream &os, const VertexAttributes &attr) {
  os << "Vertex Buffer VertexAttributes (" << attr.attributes_.size()
     << " attributes)(stride = " << attr.stride() << ")\n";
  for (u64 i = 0; i < attr.attributes_.size(); ++i) {
    os << "\tAttribute[" << i << "] with offset " <<
       attr.attributeOffset(i) << "\n";
    os << "\t\tname: " << attr.attributes_[i].name << std::endl;
    os << "\t\tsize: " << attr.attributes_[i].size << std::endl;
    os << "\t\ttype: " << OpenGL::TypeToStr(attr.attributes_[i].type) << std::endl;
    os << "\t\tlocation: " << attr.attributes_[i].location << std::endl;
    os << "\t\tdivisor: " << attr.attributes_[i].divisor << std::endl;
  }
  return os;
}

void VertexAttributes::setAttributeLocation(u64 id, GLint location) {
  attributes_[id].location = location;
}

}

