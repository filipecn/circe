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
///\file buffer_interface.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-17-10
///
///\brief

#include "model.h"
#include <circe/io/io.h>

namespace circe {

Model Model::fromFile(const hermes::Path &path, shape_options options) {
  if (path.extension() == "obj")
    return std::move(io::readOBJ(path, options | shape_options::unique_positions));
  return std::move(Model());
}

Model::Model() = default;

Model::Model(Model &&other) noexcept {
  indices_ = std::move(other.indices_);
  data_ = std::move(other.data_);
  element_type_ = other.element_type_;
}

Model::~Model() = default;

Model &Model::operator=(Model &&other) noexcept {
  indices_ = std::move(other.indices_);
  data_ = std::move(other.data_);
  element_type_ = other.element_type_;
  return *this;
}

Model &Model::operator=(const Model &other) {
  indices_ = other.indices_;
  data_ = other.data_;
  element_type_ = other.element_type_;
  return *this;
}

Model &Model::operator=(hermes::AoS &&data) {
  data_ = std::forward<hermes::AoS>(data);
  return *this;
}

Model &Model::operator=(const hermes::AoS &data) {
  data_ = data;
  return *this;
}

Model &Model::operator=(const std::vector<i32> &indices) {
  indices_ = indices;
  return *this;
}

Model &Model::operator=(const std::vector<f32> &vertex_data) {
  HERMES_NOT_IMPLEMENTED
  HERMES_UNUSED_VARIABLE(vertex_data);
  return *this;
}

void Model::resize(u64 new_size) {
  data_.resize(new_size);
}

void Model::setIndices(std::vector<i32> &&indices) {
  indices_ = indices;
}

void Model::setPrimitiveType(hermes::GeometricPrimitiveType primitive_type) {
  element_type_ = primitive_type;
}

std::ostream &operator<<(std::ostream &o, const Model &model) {
  auto ESTR = [](hermes::GeometricPrimitiveType p) -> std::string {
#define ES(P) \
    if(p == P) return #P
    ES(hermes::GeometricPrimitiveType::POINTS);
    ES(hermes::GeometricPrimitiveType::LINES);
    ES(hermes::GeometricPrimitiveType::LINE_STRIP);
    ES(hermes::GeometricPrimitiveType::LINE_LOOP);
    ES(hermes::GeometricPrimitiveType::TRIANGLES);
    ES(hermes::GeometricPrimitiveType::TRIANGLE_STRIP);
    ES(hermes::GeometricPrimitiveType::TRIANGLE_FAN);
    ES(hermes::GeometricPrimitiveType::QUADS);
    ES(hermes::GeometricPrimitiveType::TETRAHEDRA);
    ES(hermes::GeometricPrimitiveType::CUSTOM);
    return "ERR";
#undef ES
  };
  o << "Model:\n" << model.data_;
  o << "Model primitive type " << ESTR(model.element_type_) << std::endl;
  o << "Model Indices(" << model.elementCount() << " primitives):\n";
  for (auto i : model.indices_)
    o << i << " ";
  o << std::endl;
  return o;
}
hermes::bbox3 Model::boundingBox() const {
  return hermes::bbox3();
}

void Model::fitToBox(const hermes::bbox3 &box) {
  HERMES_NOT_IMPLEMENTED
  HERMES_UNUSED_VARIABLE(box)
}

u64 Model::elementCount() const {
  size_t index_count = indices_.empty() ? data_.size() : indices_.size();
  switch (element_type_) {
  case hermes::GeometricPrimitiveType::TRIANGLES: return index_count / 3;
  case hermes::GeometricPrimitiveType::LINES: return index_count / 2;
  case hermes::GeometricPrimitiveType::POINTS:
  case hermes::GeometricPrimitiveType::LINE_LOOP: return index_count;
  case hermes::GeometricPrimitiveType::LINE_STRIP: return index_count - 1;
  case hermes::GeometricPrimitiveType::TRIANGLE_STRIP:
  case hermes::GeometricPrimitiveType::TRIANGLE_FAN: return index_count - 2;
  case hermes::GeometricPrimitiveType::QUADS:
  case hermes::GeometricPrimitiveType::TETRAHEDRA: return index_count / 4;
  default: return index_count;
  }
}

}
