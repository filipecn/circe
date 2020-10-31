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
///\file scene_model.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-18-10
///
///\brief

#ifndef PONOS_CIRCE_CIRCE_GL_SCENE_SCENE_MODEL_H
#define PONOS_CIRCE_CIRCE_GL_SCENE_SCENE_MODEL_H

#include <circe/scene/model.h>

namespace circe::gl {

class SceneModel {
public:
  // ***********************************************************************
  //                          STATIC METHODS
  // ***********************************************************************
  static SceneModel fromFile(const ponos::Path &path);
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  SceneModel();
  explicit SceneModel(const Model &model);
  explicit SceneModel(Model &&model) noexcept;
  SceneModel(SceneModel &&other) noexcept;
  ~SceneModel();
  // ***********************************************************************
  //                            OPERATORS
  // ***********************************************************************
  SceneModel &operator=(SceneModel &&other) noexcept;
  SceneModel &operator=(const Model &model);
  SceneModel &operator=(Model &&model) noexcept;
  // ***********************************************************************
  //                             METHODS
  // ***********************************************************************
  inline u64 vertexCount() const { return vb_.vertexCount(); }
  inline u64 elementCount() const { return ib_.element_count; }
  VertexBuffer &vertexBuffer() { return vb_; }
  void draw();
  // ***********************************************************************
  //                          PUBLIC FIELDS
  // ***********************************************************************
  Program program;

private:
  VertexArrayObject vao_;
  VertexBuffer vb_;
  IndexBuffer ib_;
  Model model_;
};

}

#endif //PONOS_CIRCE_CIRCE_GL_SCENE_SCENE_MODEL_H
