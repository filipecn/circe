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
///\file io.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-18-10
///
///\brief

#include "io.h"

//#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include <tiny_obj_loader.h>

/// A unique vertex data is comprised with each of its component indices
struct ObjIndexKey {
  int vertex_index{0};
  int normal_index{0};
  int uv_index{0};
  bool operator==(const ObjIndexKey &other) const {
    return vertex_index == other.vertex_index &&
        normal_index == other.normal_index &&
        uv_index == other.uv_index;
  }
};

struct ObjIndexKeyHash : public std::unary_function<ObjIndexKey, int> {
  int operator()(const ObjIndexKey &k) const {
    return k.vertex_index ^ k.normal_index ^ k.uv_index;
  }
};

namespace circe {

Model io::readOBJ(const ponos::Path &path, u32 mesh_id) {
  Model model;
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.fullName().c_str())) {
    spdlog::error("Failed to load obj file {} : {}.", path.fullName(), err);
    return model;
  }
  if (!warn.empty())
    spdlog::warn("Load obj: {}", warn);

  u64 position_id{0}, normal_id{0}, color_id{0}, uv_id{0};
  if (!attrib.vertices.empty())
    position_id = model.pushAttribute<ponos::point3>("position");
  if (!attrib.normals.empty())
    normal_id = model.pushAttribute<ponos::vec3>("normal");
  if (!attrib.colors.empty())
    color_id = model.pushAttribute<ponos::vec3>("color");
  if (!attrib.texcoords.empty())
    uv_id = model.pushAttribute<ponos::point2>("uv");

  if (mesh_id >= shapes.size()) {
    spdlog::error("readOBJ: Shape not found!");
    return model;
  }

  std::vector<f32> vertex_data;
  std::vector<i32> index_data;
  /// We need to map indices, because the same vertex can have different
  /// indices for each of its elements
  std::unordered_map<const ObjIndexKey, i32, ObjIndexKeyHash> index_map;

  /// build vertex indices
  auto &shape = shapes[mesh_id];
  u64 index_offset = 0;
  u64 vertex_count = 0;
  // loop over faces
  for (auto fv : shape.mesh.num_face_vertices) {
    // loop over vertices in the face
    for (u64 v = 0; v < fv; ++v) {
      auto idx = shape.mesh.indices[index_offset + v];
      ObjIndexKey key{
          idx.vertex_index,
          idx.normal_index,
          idx.texcoord_index
      };
      auto it = index_map.find(key);
      if (it == index_map.end()) {
        index_map[key] = vertex_count;
        index_data.emplace_back(vertex_count++);
      } else
        index_data.emplace_back(it->second);
    }
    index_offset += fv;
  }
  /// decompress vertex data
  model.resize(vertex_count);
  for (const auto &vertex : index_map) {
    const auto &idx = vertex.first;
    // add new vertex
    if (!attrib.vertices.empty())
      model.attributeValue<ponos::point3>(position_id, vertex.second) = {
          attrib.vertices[3 * idx.vertex_index + 0],
          attrib.vertices[3 * idx.vertex_index + 1],
          attrib.vertices[3 * idx.vertex_index + 2]};
    if (!attrib.normals.empty())
      model.attributeValue<ponos::vec3>(normal_id, vertex.second) = {
          attrib.normals[3 * idx.normal_index + 0],
          attrib.normals[3 * idx.normal_index + 1],
          attrib.normals[3 * idx.normal_index + 2]};
    if (!attrib.colors.empty())
      model.attributeValue<ponos::vec3>(color_id, vertex.second) = {
          attrib.colors[3 * idx.vertex_index + 0],
          attrib.colors[3 * idx.vertex_index + 1],
          attrib.colors[3 * idx.vertex_index + 2]};
    if (!attrib.texcoords.empty())
      model.attributeValue<ponos::vec2>(uv_id, vertex.second) = {
          attrib.texcoords[2 * idx.uv_index + 0],
          attrib.texcoords[2 * idx.uv_index + 1]};
  }
  model.setIndices(std::move(index_data));
  return model;
}

}
