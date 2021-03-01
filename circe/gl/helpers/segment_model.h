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
///\file segment_model.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-02-17
///
///\brief

#ifndef CIRCE_CIRCE_GL_HELPERS_SEGMENT_MODEL_H
#define CIRCE_CIRCE_GL_HELPERS_SEGMENT_MODEL_H

#include <ponos/geometry/segment.h>
#include <circe/gl/scene/scene_object.h>
#include <circe/gl/scene/scene_model.h>

namespace circe::gl {

class SegmentModel : public SceneObject {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  ///
  /// \param bbox
  SegmentModel() = default;
  explicit SegmentModel(const ponos::Segment3 &s);
  ~SegmentModel() override;
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  ///
  /// \param s
  /// \return
  SegmentModel &operator=(const ponos::Segment3 &s);
  // ***********************************************************************
  //                             METHODS
  // ***********************************************************************
  ///
  /// \param camera
  /// \param t
  void draw(const CameraInterface *camera, ponos::Transform t) override;
  // ***********************************************************************
  //                          PUBLIC FIELDS
  // ***********************************************************************
  ponos::Transform transform;
  circe::Color color{1.f, 1.f, 1.f, .5f};

private:

  SceneModel mesh_;
};

}

#endif //CIRCE_CIRCE_GL_HELPERS_SEGMENT_MODEL_H
