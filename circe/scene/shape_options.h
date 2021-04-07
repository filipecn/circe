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
///\file model.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-01-03
///
///\brief

#ifndef CIRCE_CIRCE_SCENE_SHAPE_OPTIONS_H
#define CIRCE_CIRCE_SCENE_SHAPE_OPTIONS_H

#include <circe/common/bitmask_operators.h>

namespace circe {

/// Shape's mesh attributes and configurations
enum class shape_options {
  none = 0x00, //!< the mesh contains only positions
  normal = 0x01, //!< generate vertex normals
  uv = 0x02, //!< generate vertex uv coordinates
  tangent_space = 0x4, //!< generate vertex tangent space (stores tangents and bitangents)
  tangent = 0x8, //!< generate vertex tangent space (stores tangents)
  bitangent = 0x10, //!< generate vertex tangent space (stores bitangents)
  unique_positions = 0x20,  //!< vertex attributes are averaged to occupy a single index in the mesh
  wireframe = 0x40, //!< only edges
  vertices = 0x80, //!< only vertices
  flip_normals = 0x100, //!< flip normals to point inwards (uv coordinates may change as well)
  flip_faces = 0x200 //!< reverse face vertex order
};
CIRCE_ENABLE_BITMASK_OPERATORS(shape_options);
}

#endif //CIRCE_CIRCE_SCENE_SHAPE_OPTIONS_H
