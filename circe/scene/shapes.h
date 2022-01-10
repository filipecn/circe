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
///\file shapes.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-24-10
///
///\brief

#ifndef PONOS_CIRCE_CIRCE_SCENE_SHAPES_H
#define PONOS_CIRCE_CIRCE_SCENE_SHAPES_H

#include <vector>
#include <hermes/data_structures/n_mesh.h>
#include <hermes/geometry/point.h>
#include <hermes/geometry/plane.h>
#include <hermes/geometry/segment.h>
#include <circe/scene/model.h>
#include <hermes/data_structures/raw_mesh.h>

namespace circe {

class Shapes {
public:
  ///
  /// \param mesh
  /// \param options
  /// \return
  static Model fromNMesh(const hermes::NMesh &mesh,
                         shape_options options = shape_options::none);
  template<u64 N>
  static Model fromPMesh(const hermes::PMesh<N> &mesh,
                         shape_options options = shape_options::none) {
    Model model;
    // process options
    if ((options & shape_options::tangent_space) == shape_options::tangent_space)
      options = options | shape_options::tangent | shape_options::bitangent;
    const bool generate_wireframe = testMaskBit(options, shape_options::wireframe);
    const bool generate_normals = testMaskBit(options, shape_options::normal);
    bool generate_uvs = testMaskBit(options, shape_options::uv);
    const bool generate_tangents = testMaskBit(options, shape_options::tangent);
    const bool generate_bitangents = testMaskBit(options, shape_options::bitangent);

    // setup fields
    hermes::AoS aos;
    const u64 position_id = aos.pushField<hermes::point3>("position");
    const u64 normal_id = generate_normals ? aos.pushField<hermes::vec3>("normal") : 0;
    const u64 uv_id = generate_uvs ? aos.pushField<hermes::point2>("uvs") : 0;
    const u64 tangent_id = generate_tangents ? aos.pushField<hermes::vec3>("tangents") : 0;
    const u64 bitangent_id = generate_bitangents ? aos.pushField<hermes::vec3>("bitangents") : 0;

    // get a copy of positions
    auto vertex_positions = mesh.positions();

    std::vector<i32> indices;
    if (testMaskBit(options, shape_options::wireframe)) {
      model.setPrimitiveType(hermes::GeometricPrimitiveType::LINES);
      for (auto f : mesh.faces()) {
        u64 a = hermes::Numbers::greatest<u64>();
        u64 b = a;
        f.vertices(a, b);
        indices.emplace_back(a);
        indices.emplace_back(b);
      }
    } else if (testMaskBit(options, shape_options::vertices)) {
      model.setPrimitiveType(hermes::GeometricPrimitiveType::POINTS);
      for (u64 i = 0; i < mesh.vertexCount(); ++i)
        indices.emplace_back(i);
    } else {
      model.setPrimitiveType(hermes::GeometricPrimitiveType::TRIANGLES);
      for (auto c : mesh.cells()) {
        if (N == 3) {
          // the cell is already a triangle
          for (auto s : c.star())
            indices.emplace_back(s.vertexIndex());
        } else {
          // in case we have a N polygon, we create N triangles using the cell centroid
          for (auto s :c.star()) {
            // TODO check orientation
            u64 a, b;
            mesh.faceVertices(s.faceIndex(), a, b);
            indices.emplace_back(vertex_positions.size()); // centroid index
            indices.emplace_back(a);
            indices.emplace_back(b);
          }
          vertex_positions.emplace_back(c.centroid());
        }
      }
    }

    // copy vertex positions
    aos.resize(vertex_positions.size());
    auto position_field = aos.field<hermes::point3>(position_id) = vertex_positions;

    model = std::move(aos);
    model = indices;
    return std::move(model);
  }
  /// Generates a new model with shape options
  /// \param model input model
  /// \param options
  /// \return
  static Model convert(const Model &model, shape_options options,
                       const std::vector<u64> &attr_filter = {});
  ///
  /// \param center
  /// \param radius
  /// \param divisions
  /// \param options
  /// \return
  static Model icosphere(const hermes::point3 &center, real_t radius,
                         u32 divisions, shape_options options = shape_options::none);
  ///
  /// \param divisions
  /// \param options
  /// \return
  static Model icosphere(u32 divisions, shape_options options = shape_options::none);
  ///
  /// \param plane
  /// \param center
  /// \param extension
  /// \param divisions
  /// \param options
  /// \return
  static Model plane(const hermes::Plane &plane,
                     const hermes::point3 &center,
                     const hermes::vec3 &extension,
                     u32 divisions, shape_options options = shape_options::none);
  static Model plane(const hermes::Plane &plane,
                     const hermes::point3 &center,
                     const hermes::vec3 &direction,
                     const hermes::vec2 &size,
                     hermes::size2 divisions, shape_options options = shape_options::none);
  ///
  /// \param box
  /// \param options
  /// \return
  static Model box(const hermes::bbox3 &box, shape_options options = shape_options::none);
  static Model box(const hermes::bbox2 &box, shape_options options = shape_options::none);
  ///
  /// \param s
  /// \param options
  /// \return
  static Model segment(const hermes::Segment3 &s,
                       shape_options options = shape_options::none);

  static Model curve(const std::vector<hermes::point3> &vertices, shape_options options = shape_options::none);
};

}

#endif //PONOS_CIRCE_CIRCE_SCENE_SHAPES_H
