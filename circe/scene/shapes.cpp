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

#include <circe/scene/shapes.h>

namespace circe {

Model Shapes::icosphere(const ponos::point3 &center, real_t radius, u32 divisions, shape_options options) {
  ponos::AoS aos;
  aos.pushField<ponos::point3>("position");
  u64 struct_size = 3;
  if ((options & shape_options::normal) == shape_options::normal) {
    aos.pushField<ponos::vec3>("normal");
    struct_size += 3;
  }
  if ((options & shape_options::uv) == shape_options::uv) {
    aos.pushField<ponos::point2>("uv");
    struct_size += 2;
  }
  // starting sphere
  float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
  std::vector<real_t> vertex_data(12 * struct_size);
#define STORE_POSITION(INDEX, X, Y, Z) \
  *(vertex_data.data() + struct_size * (INDEX) + 0) = X; \
  *(vertex_data.data() + struct_size * (INDEX) + 1) = Y; \
  *(vertex_data.data() + struct_size * (INDEX) + 2) = Z;
#define ADD_FACE(A, B, C) \
            index_data.emplace_back(A);              \
            index_data.emplace_back(B);              \
            index_data.emplace_back(C);
  STORE_POSITION(0, -1, t, 0);
  STORE_POSITION(1, 1, t, 0);
  STORE_POSITION(2, -1, -t, 0);
  STORE_POSITION(3, 1, -t, 0);
  STORE_POSITION(4, 0, -1, t);
  STORE_POSITION(5, 0, 1, t);
  STORE_POSITION(6, 0, -1, -t);
  STORE_POSITION(7, 0, 1, -t);
  STORE_POSITION(8, t, 0, -1);
  STORE_POSITION(9, t, 0, 1);
  STORE_POSITION(10, -t, 0, -1);
  STORE_POSITION(11, -t, 0, 1);
  std::vector<i32> index_data = {0, 11, 5,/**/ 0, 5, 1,/**/0, 1, 7,/**/0, 7, 10,/**/0, 10, 11,/**/
                                 1, 5, 9,/**/  5, 11, 4,/**/11, 10, 2,/**/10, 7, 6,/**/7, 1, 8,/**/
                                 3, 9, 4,/**/3, 4, 2,/**/3, 2, 6,/**/3, 6, 8,/**/3, 8, 9,/**/
                                 4, 9, 5,/**/2, 4, 11,/**/6, 2, 10,/**/8, 6, 7,/**/9, 8, 1};
  // refine mesh
  std::map<std::pair<u32, u32>, u64> indices_cache;
  std::function<u64(u32, u32)> midPoint = [&](u32 a, u32 b) -> u64 {
    std::pair<u32, u32> key(std::min(a, b), std::max(a, b));
    u64 n = vertex_data.size() / struct_size;
    if (indices_cache.find(key) != indices_cache.end())
      return indices_cache[key];
    const ponos::point3 &pa = *reinterpret_cast<ponos::point3 *>(vertex_data.data() + struct_size * a);
    const ponos::point3 &pb = *reinterpret_cast<ponos::point3 *>(vertex_data.data() + struct_size * b);
    auto pm = pa + (pb - pa) * 0.5f;
    vertex_data.resize(vertex_data.size() + struct_size);
    STORE_POSITION(n, pm.x, pm.y, pm.z);
    return n;
  };
  for (u32 i = 0; i < divisions; i++) {
    u64 n = index_data.size() / 3;
    for (u64 j = 0; j < n; j++) {
      i32 v0 = index_data[j * 3 + 0];
      i32 v1 = index_data[j * 3 + 1];
      i32 v2 = index_data[j * 3 + 2];
      u64 a = midPoint(v0, v1);
      u64 b = midPoint(v1, v2);
      u64 c = midPoint(v2, v0);
      ADD_FACE(v0, a, c);
      ADD_FACE(v1, b, a);
      ADD_FACE(v2, c, b);
      ADD_FACE(a, c, b);
    }
  }
  // finish data
  u64 vertex_count = vertex_data.size() / struct_size;
  for (u64 i = 0; i < vertex_count; ++i) {
    // project vertex to unit sphere
    (*reinterpret_cast<ponos::vec3 *>(vertex_data.data() + struct_size * i)).normalize();
    // compute normal
    if ((options & shape_options::normal) == shape_options::normal)
      (*reinterpret_cast<ponos::vec3 *>(vertex_data.data() + struct_size * i + 3)) =
          (*reinterpret_cast<ponos::vec3 *>(vertex_data.data() + struct_size * i));
    // compute uv
    if ((options & shape_options::uv) == shape_options::uv) {
      (*reinterpret_cast<ponos::point2 *>(vertex_data.data() + struct_size * i + 6)).x = std::atan2(
          (*reinterpret_cast<ponos::vec3 *>(vertex_data.data() + struct_size * i)).y,
          (*reinterpret_cast<ponos::vec3 *>(vertex_data.data() + struct_size * i)).x);
      (*reinterpret_cast<ponos::point2 *>(vertex_data.data() + struct_size * i + 6)).y = std::acos(
          (*reinterpret_cast<ponos::vec3 *>(vertex_data.data() + struct_size * i)).z);
    }
    // translate and scale
    (*reinterpret_cast<ponos::point3 *>(vertex_data.data() + struct_size * i)).x * radius + center.x;
    (*reinterpret_cast<ponos::point3 *>(vertex_data.data() + struct_size * i)).y * radius + center.y;
    (*reinterpret_cast<ponos::point3 *>(vertex_data.data() + struct_size * i)).z * radius + center.z;
  }
  aos = std::move(vertex_data);
  Model model;
  model = std::move(aos);
  model = index_data;
  return model;
#undef STORE_POSITION
#undef ADD_FACE
}

Model Shapes::icosphere(u32 divisions, shape_options options) {
  return std::forward<Model>(icosphere(ponos::point3(), 1, divisions, options));
}

Model Shapes::plane(const ponos::Plane &plane,
                    const ponos::point3 &center,
                    const ponos::vec3 &extension,
                    u32 divisions,
                    shape_options options) {
  if ((options & shape_options::tangent_space) == shape_options::tangent_space)
    options = options | shape_options::tangent | shape_options::bitangent;
  const bool generate_normals = (options & shape_options::normal) == shape_options::normal;
  bool generate_uvs = (options & shape_options::uv) == shape_options::uv;
  const bool generate_tangents = (options & shape_options::tangent) == shape_options::tangent;
  const bool generate_bitangents = (options & shape_options::bitangent) == shape_options::bitangent;
  if (std::fabs(dot(plane.normal, extension)) > 1e-8)
    spdlog::warn("Extension vector must be normal to plane normal vector.");
  // if the tangent space is needed, uv must be generated as well
  if (!generate_uvs && (generate_tangents || generate_bitangents)) {
    spdlog::warn("UV will be generated since tangent space is being generated.");
    generate_uvs = true;
  }
  ponos::AoS aos;
  const u64 position_id = aos.pushField<ponos::point3>("position");
  const u64 normal_id = generate_normals ? aos.pushField<ponos::vec3>("normal") : 0;
  const u64 uv_id = generate_uvs ? aos.pushField<ponos::point2>("uvs") : 0;
  const u64 tangent_id = generate_tangents ? aos.pushField<ponos::vec3>("tangents") : 0;
  const u64 bitangent_id = generate_bitangents ? aos.pushField<ponos::vec3>("bitangents") : 0;

  aos.resize((divisions + 1) * (divisions + 1));

  auto half_size = extension.length() / 2;
  f32 div_rec = 1.0 / divisions;
  f32 step = (half_size * 2) * div_rec;
  auto dx = normalize(extension);
  auto dy = normalize(cross(ponos::vec3(plane.normal), dx));
  auto origin = center - dx * half_size - dy * half_size;
  u64 vertex_index = 0;
  for (u32 x = 0; x <= divisions; ++x)
    for (u32 y = 0; y <= divisions; ++y) {
      auto p = origin + dx * step * static_cast<float>(x) + dy * step * static_cast<float>(y);
      aos.valueAt<ponos::point3>(position_id, vertex_index) = p;
      if (generate_normals)
        aos.valueAt<ponos::normal3>(normal_id, vertex_index) = plane.normal;
      if (generate_uvs)
        aos.valueAt<ponos::point2>(uv_id, vertex_index) = {x * div_rec, y * div_rec};
      vertex_index++;
    }
  u64 w = divisions + 1;
  std::vector<i32> index_data;
  for (u64 i = 0; i < divisions; ++i)
    for (u64 j = 0; j < divisions; ++j) {
      index_data.emplace_back(i * w + j);
      index_data.emplace_back((i + 1) * w + j);
      index_data.emplace_back(i * w + j + 1);
      index_data.emplace_back(i * w + j + 1);
      index_data.emplace_back((i + 1) * w + j);
      index_data.emplace_back((i + 1) * w + j + 1);
    }
  // compute tangent space
  if (generate_tangents || generate_bitangents) {
    auto face_count = index_data.size() / 3;
    for (u64 i = 0; i < face_count; ++i) {
      auto edge_a = aos.valueAt<ponos::point3>(position_id, index_data[i * 3 + 1])
          - aos.valueAt<ponos::point3>(position_id, index_data[i * 3 + 0]);
      auto edge_b = aos.valueAt<ponos::point3>(position_id, index_data[i * 3 + 2])
          - aos.valueAt<ponos::point3>(position_id, index_data[i * 3 + 0]);
      auto delta_uv_a = aos.valueAt<ponos::point2>(uv_id, index_data[i * 3 + 1])
          - aos.valueAt<ponos::point2>(uv_id, index_data[i * 3 + 0]);
      auto delta_uv_b = aos.valueAt<ponos::point2>(uv_id, index_data[i * 3 + 2])
          - aos.valueAt<ponos::point2>(uv_id, index_data[i * 3 + 0]);
      f32 f = 1.0f / (delta_uv_a.x * delta_uv_b.y - delta_uv_b.x * delta_uv_a.y);
      if (generate_tangents) {
        ponos::vec3 tangent(
            f * (delta_uv_b.y * edge_a.x - delta_uv_a.y * edge_b.x),
            f * (delta_uv_b.y * edge_a.y - delta_uv_a.y * edge_b.y),
            f * (delta_uv_b.y * edge_a.z - delta_uv_a.y * edge_b.z)
        );
        tangent.normalize();
        aos.valueAt<ponos::vec3>(tangent_id, index_data[i * 3 + 0]) = tangent;
        aos.valueAt<ponos::vec3>(tangent_id, index_data[i * 3 + 1]) = tangent;
        aos.valueAt<ponos::vec3>(tangent_id, index_data[i * 3 + 2]) = tangent;
      }
      if (generate_bitangents) {
        ponos::vec3 bitangent(
            f * (-delta_uv_b.x * edge_a.x - delta_uv_a.x * edge_b.x),
            f * (-delta_uv_b.x * edge_a.y - delta_uv_a.x * edge_b.y),
            f * (-delta_uv_b.x * edge_a.z - delta_uv_a.x * edge_b.z)
        );
        bitangent.normalize();
        aos.valueAt<ponos::vec3>(bitangent_id, index_data[i * 3 + 0]) = bitangent;
        aos.valueAt<ponos::vec3>(bitangent_id, index_data[i * 3 + 1]) = bitangent;
        aos.valueAt<ponos::vec3>(bitangent_id, index_data[i * 3 + 2]) = bitangent;
      }
    }
  }
  Model model;
  model = std::move(aos);
  model = index_data;
  return model;
#undef ADD_FACE
}

}