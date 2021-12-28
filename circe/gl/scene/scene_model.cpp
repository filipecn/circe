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
///\file scene_model.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-18-10
///
///\brief

#include "scene_model.h"

namespace circe::gl {

SceneModel SceneModel::fromFile(const hermes::Path &path, shape_options options) {
  auto model = Model::fromFile(path, options);
  SceneModel scene_model;
  scene_model.vb_ = model.data();
  scene_model.ib_.element_type = OpenGL::PrimitiveToGL(model.primitiveType());
  scene_model.ib_ = model.indices();
  scene_model.primitive_count_ = scene_model.ib_.element_count ? scene_model.ib_.element_count :
                                 OpenGL::primitiveCount(scene_model.ib_.element_type, model.data().size());
  scene_model.model_ = std::move(model);
  scene_model.vao_.bind();
  scene_model.vb_.bind();
  scene_model.vb_.bindAttributeFormats();
  scene_model.vao_.unbind();
  return std::move(scene_model);
}

SceneModel::SceneModel() = default;

SceneModel::SceneModel(SceneModel &&other) noexcept {
  model_ = std::forward<Model>(model_);
  vao_ = std::move(other.vao_);
  vb_ = std::move(other.vb_);
  ib_ = std::move(other.ib_);
  primitive_count_ = other.primitive_count_;
}

SceneModel::SceneModel(const Model &model) {
  model_ = model;
  vb_ = model.data();
  ib_.element_type = OpenGL::PrimitiveToGL(model_.primitiveType());
  ib_ = model.indices();
  primitive_count_ = ib_.element_count ? ib_.element_count :
                     OpenGL::primitiveCount(ib_.element_type, vb_.vertexCount());
  vao_.bind();
  vb_.bind();
  vb_.bindAttributeFormats();
  vao_.unbind();
}

SceneModel::SceneModel(Model &&model) noexcept {
  model_ = std::forward<Model>(model);
  vb_ = model_.data();
  ib_.element_type = OpenGL::PrimitiveToGL(model_.primitiveType());
  ib_ = model_.indices();
  primitive_count_ = ib_.element_count ? ib_.element_count :
                     OpenGL::primitiveCount(ib_.element_type, vb_.vertexCount());
  vao_.bind();
  vb_.bind();
  vb_.bindAttributeFormats();
  vao_.unbind();
}

SceneModel::~SceneModel() = default;

SceneModel &SceneModel::operator=(SceneModel &&other) noexcept {
  model_ = std::move(other.model_);
  vao_ = std::move(other.vao_);
  vb_ = std::move(other.vb_);
  ib_ = std::move(other.ib_);
  primitive_count_ = other.primitive_count_;
  return *this;
}

SceneModel &SceneModel::operator=(const Model &model) {
  model_ = model;
  vb_ = model.data();
  ib_.element_type = OpenGL::PrimitiveToGL(model.primitiveType());
  ib_ = model.indices();
  primitive_count_ = ib_.element_count ? ib_.element_count :
                     OpenGL::primitiveCount(ib_.element_type, vb_.vertexCount());
  vao_.bind();
  vb_.bind();
  vb_.bindAttributeFormats();
  vao_.unbind();
  return *this;
}

SceneModel &SceneModel::operator=(Model &&model) noexcept {
  model_ = std::forward<Model>(model);
  vb_ = model_.data();
  ib_.element_type = OpenGL::PrimitiveToGL(model_.primitiveType());
  ib_ = model_.indices();
  primitive_count_ = ib_.element_count ? ib_.element_count :
                     OpenGL::primitiveCount(ib_.element_type, vb_.vertexCount());
  vao_.bind();
  vb_.bind();
  vb_.bindAttributeFormats();
  vao_.unbind();
  return *this;
}

void SceneModel::bind() {
  vao_.bind();
}

void SceneModel::unbind() {
  vao_.unbind();
}

void SceneModel::bindBuffers() {
  vao_.bind();
  vb_.bind();
  ib_.bind();
}

void SceneModel::draw() {
  vao_.bind();
  vb_.bind();
  if (ib_.element_count)
    ib_.draw();
  else {
    glDrawArrays(ib_.element_type, 0, vb_.vertexCount());
    CHECK_GL_ERRORS
  }
}

}
