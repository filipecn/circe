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

Model Model::fromFile(const ponos::Path &path, shape_options options) {
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

Model &Model::operator=(ponos::AoS &&data) {
  data_ = std::forward<ponos::AoS>(data);
  return *this;
}

Model &Model::operator=(const ponos::AoS &data) {
  data_ = data;
  return *this;
}

Model &Model::operator=(const std::vector<i32> &indices) {
  indices_ = indices;
  return *this;
}

Model &Model::operator=(const std::vector<f32> &vertex_data) {
  return *this;
}

void Model::resize(u64 new_size) {
  data_.resize(new_size);
}

void Model::setIndices(std::vector<i32> &&indices) {
  indices_ = indices;
}

void Model::setPrimitiveType(ponos::GeometricPrimitiveType primitive_type) {
  element_type_ = primitive_type;
}

std::ostream &operator<<(std::ostream &o, const Model &model) {
  auto ESTR = [](ponos::GeometricPrimitiveType p) -> std::string {
#define ES(P) \
    if(p == P) return #P
    ES(ponos::GeometricPrimitiveType::POINTS);
    ES(ponos::GeometricPrimitiveType::LINES);
    ES(ponos::GeometricPrimitiveType::LINE_STRIP);
    ES(ponos::GeometricPrimitiveType::LINE_LOOP);
    ES(ponos::GeometricPrimitiveType::TRIANGLES);
    ES(ponos::GeometricPrimitiveType::TRIANGLE_STRIP);
    ES(ponos::GeometricPrimitiveType::TRIANGLE_FAN);
    ES(ponos::GeometricPrimitiveType::QUADS);
    ES(ponos::GeometricPrimitiveType::TETRAHEDRA);
    ES(ponos::GeometricPrimitiveType::CUSTOM);
    return "ERR";
#undef ES
  };
  o << "Model:\n" << model.data_;
  o << "Model primitive type " << ESTR(model.element_type_) << std::endl;
  o << "Model Indices:\n";
  for (auto i : model.indices_)
    o << i << " ";
  o << std::endl;
  return o;
}
ponos::bbox3 Model::boundingBox() const {
  return ponos::bbox3();
}

void Model::fitToBox(const ponos::bbox3 &box) {

}
u64 Model::elementCount() const {
  if (indices_.empty())
    return 0;
  switch (element_type_) {
  case ponos::GeometricPrimitiveType::TRIANGLES: return indices_.size() / 3;
  case ponos::GeometricPrimitiveType::LINES: return indices_.size() / 2;
  case ponos::GeometricPrimitiveType::POINTS:
  case ponos::GeometricPrimitiveType::LINE_LOOP: return indices_.size();
  case ponos::GeometricPrimitiveType::LINE_STRIP: return indices_.size() - 1;
  case ponos::GeometricPrimitiveType::TRIANGLE_STRIP:
  case ponos::GeometricPrimitiveType::TRIANGLE_FAN: return indices_.size() - 2;
  case ponos::GeometricPrimitiveType::QUADS:
  case ponos::GeometricPrimitiveType::TETRAHEDRA: return indices_.size() / 4;
  default: return indices_.size();
  }
}

}
