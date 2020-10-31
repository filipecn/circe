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
#include <circe/io.h>

namespace circe {

Model Model::fromFile(const ponos::Path &path) {
  if (path.extension() == "obj")
    return std::move(io::readOBJ(path));
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

std::ostream &operator<<(std::ostream &o, const Model &model) {
  o << "Model:\n" << model.data_;
  o << "Model Indices:\n";
  for (auto i : model.indices_)
    o << i << " ";
  o << std::endl;
  return o;
}

}
