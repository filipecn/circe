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
#include <circe/scene/model.h>

using namespace hermes;

namespace circe {

Model Shapes::fromNMesh(const NMesh &mesh, shape_options options) {
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
  AoS aos;
  const u64 position_id = aos.pushField<point3>("position");
  const u64 normal_id = generate_normals ? aos.pushField<vec3>("normal") : 0;
  const u64 uv_id = generate_uvs ? aos.pushField<point2>("uvs") : 0;
  const u64 tangent_id = generate_tangents ? aos.pushField<vec3>("tangents") : 0;
  const u64 bitangent_id = generate_bitangents ? aos.pushField<vec3>("bitangents") : 0;

  // get a copy of positions
  auto vertex_positions = mesh.positions();

  std::vector<i32> indices;
  if (testMaskBit(options, shape_options::wireframe)) {
    model.setPrimitiveType(GeometricPrimitiveType::LINES);
    for (auto f : mesh.faces()) {
      u64 a = Numbers::greatest<u64>();
      u64 b = a;
      f.vertices(a, b);
      indices.emplace_back(a);
      indices.emplace_back(b);
    }
  } else if (testMaskBit(options, shape_options::vertices)) {
    model.setPrimitiveType(GeometricPrimitiveType::POINTS);
    for (u64 i = 0; i < mesh.vertexCount(); ++i)
      indices.emplace_back(i);
  } else {
    model.setPrimitiveType(GeometricPrimitiveType::TRIANGLES);
    for (auto c : mesh.cells()) {
      if (mesh.cellFaceCount(c.index) == 3) {
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
  auto position_field = aos.field<point3>(position_id) = vertex_positions;

  model = std::move(aos);
  model = indices;
  return std::move(model);
}

Model Shapes::convert(const Model &model, shape_options options, const std::vector<u64> &attr_filter) {
  Model converted_model;
  // filter attributes
  if (attr_filter.empty())
    converted_model = model.data();
  else {
    AoS aos;
    // add fields
    const auto &fields = model.data().fields();
    for (const auto &field_id : attr_filter) {
#define ADD_FIELD(D, C, T) \
      if(fields[field_id].type == D && fields[field_id].component_count == C) \
        aos.pushField<T>(fields[field_id].name);
      ADD_FIELD(DataType::F32, 1, f32)
      ADD_FIELD(DataType::F32, 2, vec2f)
      ADD_FIELD(DataType::F32, 3, vec3f)
#undef ADD_FIELD
    }
    aos.resize(model.data().size());
    // copy fields
    u64 f = 0;
    for (const auto &field_id : attr_filter) {
#define CPY_FIELD(D, C, T) \
      if(fields[field_id].type == D && fields[field_id].component_count == C) { \
        const auto& field = model.data().field<T>(fields[field_id].name);       \
        for(u64 i = 0; i < aos.size(); ++i)                                     \
          aos.valueAt<T>(f, i) = field[i];}
      CPY_FIELD(DataType::F32, 1, f32)
      CPY_FIELD(DataType::F32, 2, vec2f)
      CPY_FIELD(DataType::F32, 3, vec3f)
#undef CPY_FIELD
      f++;
    }
  }
  if (testMaskBit(options, shape_options::wireframe)) {
    if (model.primitiveType() == GeometricPrimitiveType::LINES ||
        model.primitiveType() == GeometricPrimitiveType::POINTS ||
        model.primitiveType() == GeometricPrimitiveType::LINE_LOOP ||
        model.primitiveType() == GeometricPrimitiveType::LINE_STRIP)
      converted_model = model.indices();
    else {
      converted_model.setPrimitiveType(GeometricPrimitiveType::LINES);
      u32 element_size = 3;
      switch (model.primitiveType()) {
      case GeometricPrimitiveType::TRIANGLES: element_size = 3;
        break;
      case GeometricPrimitiveType::QUADS: element_size = 4;
        break;
      default:Log::warn("GeometricPrimitiveType not implemented in circe::Shapes::convert");
      }
      const auto &model_indices = model.indices();
      u64 element_count = model.elementCount();
      HERMES_ASSERT(element_count * element_size <= model_indices.size());
      std::set<std::pair<u64, u64>> edges;
      for (u64 e = 0; e < element_count; ++e)
        for (u32 k = 0; k < element_size; ++k) {
          u64 a = std::min(model_indices[e * element_size + k],
                           model_indices[e * element_size + (k + 1) % element_size]);
          u64 b = std::max(model_indices[e * element_size + k],
                           model_indices[e * element_size + (k + 1) % element_size]);
          edges.insert(std::make_pair(a, b));
        }
      std::vector<i32> indices;
      for (auto e : edges) {
        indices.emplace_back(e.first);
        indices.emplace_back(e.second);
      }
      converted_model = indices;
    }
  }
  return std::move(converted_model);
}

Model Shapes::icosphere(const point3 &center, real_t radius, u32 divisions, shape_options options) {
  AoS aos;
  aos.pushField<point3>("position");
  u64 struct_size = 3;
  if ((options & shape_options::normal) == shape_options::normal) {
    aos.pushField<vec3>("normal");
    struct_size += 3;
  }
  if ((options & shape_options::uv) == shape_options::uv) {
    aos.pushField<point2>("uv");
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
    const point3 &pa = *reinterpret_cast<point3 *>(vertex_data.data() + struct_size * a);
    const point3 &pb = *reinterpret_cast<point3 *>(vertex_data.data() + struct_size * b);
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
    (*reinterpret_cast<vec3 *>(vertex_data.data() + struct_size * i)).normalize();
    // compute normal
    if ((options & shape_options::normal) == shape_options::normal)
      (*reinterpret_cast<vec3 *>(vertex_data.data() + struct_size * i + 3)) =
          (*reinterpret_cast<vec3 *>(vertex_data.data() + struct_size * i));
    // compute uv
    if ((options & shape_options::uv) == shape_options::uv) {
      (*reinterpret_cast<point2 *>(vertex_data.data() + struct_size * i + 6)).x = std::atan2(
          (*reinterpret_cast<vec3 *>(vertex_data.data() + struct_size * i)).y,
          (*reinterpret_cast<vec3 *>(vertex_data.data() + struct_size * i)).x);
      (*reinterpret_cast<point2 *>(vertex_data.data() + struct_size * i + 6)).y = std::acos(
          (*reinterpret_cast<vec3 *>(vertex_data.data() + struct_size * i)).z);
    }
    // translate and scale
    vertex_data[i * struct_size + 0] =
        (*reinterpret_cast<point3 *>(vertex_data.data() + struct_size * i)).x * radius + center.x;
    vertex_data[i * struct_size + 1] =
        (*reinterpret_cast<point3 *>(vertex_data.data() + struct_size * i)).y * radius + center.y;
    vertex_data[i * struct_size + 2] =
        (*reinterpret_cast<point3 *>(vertex_data.data() + struct_size * i)).z * radius + center.z;
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
  return std::forward<Model>(icosphere(point3(), 1, divisions, options));
}

Model Shapes::plane(const Plane &plane,
                    const point3 &center,
                    const vec3 &extension,
                    u32 divisions,
                    shape_options options) {
  if ((options & shape_options::tangent_space) == shape_options::tangent_space)
    options = options | shape_options::tangent | shape_options::bitangent;
  const bool generate_normals = (options & shape_options::normal) == shape_options::normal;
  bool generate_uvs = (options & shape_options::uv) == shape_options::uv;
  const bool generate_tangents = (options & shape_options::tangent) == shape_options::tangent;
  const bool generate_bitangents = (options & shape_options::bitangent) == shape_options::bitangent;
  if (std::fabs(dot(plane.normal, extension)) > 1e-8)
    Log::warn("Extension vector must be normal to plane normal vector.");
  // if the tangent space is needed, uv must be generated as well
  if (!generate_uvs && (generate_tangents || generate_bitangents)) {
    Log::warn("UV will be generated since tangent space is being generated.");
    generate_uvs = true;
  }
  AoS aos;
  const u64 position_id = aos.pushField<point3>("position");
  const u64 normal_id = generate_normals ? aos.pushField<vec3>("normal") : 0;
  const u64 uv_id = generate_uvs ? aos.pushField<point2>("uvs") : 0;
  const u64 tangent_id = generate_tangents ? aos.pushField<vec3>("tangents") : 0;
  const u64 bitangent_id = generate_bitangents ? aos.pushField<vec3>("bitangents") : 0;

  aos.resize((divisions + 1) * (divisions + 1));

  auto half_size = extension.length() / 2;
  f32 div_rec = 1.0 / divisions;
  f32 step = (half_size * 2) * div_rec;
  auto dx = normalize(extension);
  auto dy = normalize(cross(vec3(plane.normal), dx));
  auto origin = center - dx * half_size - dy * half_size;
  u64 vertex_index = 0;
  for (u32 x = 0; x <= divisions; ++x)
    for (u32 y = 0; y <= divisions; ++y) {
      auto p = origin + dx * step * static_cast<float>(x) + dy * step * static_cast<float>(y);
      aos.valueAt<point3>(position_id, vertex_index) = p;
      if (generate_normals)
        aos.valueAt<normal3>(normal_id, vertex_index) = plane.normal;
      if (generate_uvs)
        aos.valueAt<point2>(uv_id, vertex_index) = {x * div_rec, y * div_rec};
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
      auto edge_a = aos.valueAt<point3>(position_id, index_data[i * 3 + 1])
          - aos.valueAt<point3>(position_id, index_data[i * 3 + 0]);
      auto edge_b = aos.valueAt<point3>(position_id, index_data[i * 3 + 2])
          - aos.valueAt<point3>(position_id, index_data[i * 3 + 0]);
      auto delta_uv_a = aos.valueAt<point2>(uv_id, index_data[i * 3 + 1])
          - aos.valueAt<point2>(uv_id, index_data[i * 3 + 0]);
      auto delta_uv_b = aos.valueAt<point2>(uv_id, index_data[i * 3 + 2])
          - aos.valueAt<point2>(uv_id, index_data[i * 3 + 0]);
      f32 f = 1.0f / (delta_uv_a.x * delta_uv_b.y - delta_uv_b.x * delta_uv_a.y);
      if (generate_tangents) {
        vec3 tangent(
            f * (delta_uv_b.y * edge_a.x - delta_uv_a.y * edge_b.x),
            f * (delta_uv_b.y * edge_a.y - delta_uv_a.y * edge_b.y),
            f * (delta_uv_b.y * edge_a.z - delta_uv_a.y * edge_b.z)
        );
        tangent.normalize();
        aos.valueAt<vec3>(tangent_id, index_data[i * 3 + 0]) = tangent;
        aos.valueAt<vec3>(tangent_id, index_data[i * 3 + 1]) = tangent;
        aos.valueAt<vec3>(tangent_id, index_data[i * 3 + 2]) = tangent;
      }
      if (generate_bitangents) {
        vec3 bitangent(
            f * (-delta_uv_b.x * edge_a.x - delta_uv_a.x * edge_b.x),
            f * (-delta_uv_b.x * edge_a.y - delta_uv_a.x * edge_b.y),
            f * (-delta_uv_b.x * edge_a.z - delta_uv_a.x * edge_b.z)
        );
        bitangent.normalize();
        aos.valueAt<vec3>(bitangent_id, index_data[i * 3 + 0]) = bitangent;
        aos.valueAt<vec3>(bitangent_id, index_data[i * 3 + 1]) = bitangent;
        aos.valueAt<vec3>(bitangent_id, index_data[i * 3 + 2]) = bitangent;
      }
    }
  }
  Model model;
  model = std::move(aos);
  model = index_data;
  return model;
#undef ADD_FACE
}

Model Shapes::box(const bbox3 &box, shape_options options) {
  if (testMaskBit(options, shape_options::tangent_space))
    options = options | shape_options::tangent | shape_options::bitangent;
  const bool generate_wireframe = testMaskBit(options, shape_options::wireframe);
  const bool only_vertices = testMaskBit(options, shape_options::vertices);
  const bool generate_normals = testMaskBit(options, shape_options::normal);
  bool generate_uvs = testMaskBit(options, shape_options::uv);
  const bool generate_tangents = testMaskBit(options, shape_options::tangent);
  const bool generate_bitangents = testMaskBit(options, shape_options::bitangent);
  const bool flip_normals = testMaskBit(options, shape_options::flip_normals);
  const bool flip_faces = testMaskBit(options, shape_options::flip_faces);
  const bool unique_positions = testMaskBit(options, shape_options::unique_positions);

  Model model;

  const u64 position_id = model.pushAttribute<point3>("position");
  const u64 normal_id = generate_normals ? model.pushAttribute<vec3>("normal") : 0;
  const u64 uv_id = generate_uvs ? model.pushAttribute<point2>("uvs") : 0;
  const u64 tangent_id = generate_tangents ? model.pushAttribute<vec3>("tangent") : 0;
  const u64 bitangent_id = generate_bitangents ? model.pushAttribute<vec3>("bitangent") : 0;

  // base vertices
  point3 base_vertices[8] = {
      {box.lower.x, box.lower.y, box.lower.z}, // 0
      {box.upper.x, box.lower.y, box.lower.z}, // 1
      {box.upper.x, box.upper.y, box.lower.z}, // 2
      {box.lower.x, box.upper.y, box.lower.z}, // 3
      {box.lower.x, box.lower.y, box.upper.z}, // 4
      {box.upper.x, box.lower.y, box.upper.z}, // 5
      {box.upper.x, box.upper.y, box.upper.z}, // 6
      {box.lower.x, box.upper.y, box.upper.z}};// 7
  // base uvs
  point2 base_uvs[4] = {
      {0, 0}, // 0
      {1, 0}, // 1
      {1, 1}, // 2
      {0, 1}, // 3
  };
  vec3 base_normals[6] = {
      {0, 0, -1}, // -Z
      {0, 0, 1}, // +Z
      {0, -1, 0}, // -Y
      {0, 1, 0}, // Y
      {-1, 0, 0}, // -X
      {1, 0, 0}, // X
  };

  std::vector<i32> base_vertex_indices = {
      0, 1, 2, 3, // -Z face
      4, 7, 6, 5, // +Z face
      0, 4, 5, 1, // -Y face
      3, 2, 6, 7, // +Y face
      0, 3, 7, 4, // -X face
      2, 1, 5, 6  // +X face
  };

  //           7
  //  3                     6
  //               2
  //
  //           4
  //  0                     5
  //               1

  std::vector<i32> indices;
  if (only_vertices) {
    model.setPrimitiveType(GeometricPrimitiveType::POINTS);
    model.resize(8);
    for (int i = 0; i < 8; ++i) {
      model.attributeValue<point3>(position_id, i) = base_vertices[i];
      indices.emplace_back(i);
    }
  } else if (unique_positions || (!generate_normals && !generate_uvs)) {
    model.resize(8);
    for (int i = 0; i < 8; ++i)
      model.attributeValue<point3>(position_id, i) = base_vertices[i];
    if (generate_wireframe) {
      model.setPrimitiveType(GeometricPrimitiveType::LINES);
      indices = {0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 5, 6, 6, 7, 7, 4};
    } else {
      model.setPrimitiveType(GeometricPrimitiveType::TRIANGLES);
      for (int f = 0; f < 6; ++f)
        for (int jump = 0; jump < 2; ++jump) {
          indices.emplace_back(base_vertex_indices[f * 4 + 0]);
          indices.emplace_back(base_vertex_indices[f * 4 + jump + 1]);
          indices.emplace_back(base_vertex_indices[f * 4 + jump + 2]);
        }
    }
  } else {
    if (generate_wireframe) {
      model.setPrimitiveType(GeometricPrimitiveType::LINES);
      indices = {0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 5, 6, 6, 7, 7, 4};
    } else if (generate_normals || generate_uvs) {
      model.setPrimitiveType(GeometricPrimitiveType::TRIANGLES);
      for (int f = 0; f < 6; ++f)
        for (int jump = 0; jump < 2; ++jump) {
          indices.emplace_back(f * 4 + 0);
          indices.emplace_back(f * 4 + jump + 1);
          indices.emplace_back(f * 4 + jump + 2);
        }
      // 4 vertices per cube quad face
      model.resize(base_vertex_indices.size());
      for (size_t i = 0; i < base_vertex_indices.size(); ++i)
        model.attributeValue<point3>(position_id, i) =
            base_vertices[base_vertex_indices[i]];
      if (generate_uvs) {
      }
      if (generate_normals) {
        for (int f = 0; f < 6; ++f)
          for (int i = 0; i < 4; ++i)
            model.attributeValue<vec3>(normal_id, f * 4 + i) =
                flip_normals ? -base_normals[f] : base_normals[f];
      }
      if (generate_tangents) {
      }
      if (generate_bitangents) {
      }

    } else {
      model.setPrimitiveType(GeometricPrimitiveType::QUADS);
      indices = base_vertex_indices;
      model.resize(8);
      for (int i = 0; i < 8; ++i)
        model.attributeValue<point3>(position_id, i) = base_vertices[i];
    }
  }
  model = indices;
  return model;
}

Model Shapes::box(const bbox2 &box, shape_options options) {
  if (testMaskBit(options, shape_options::tangent_space))
    options = options | shape_options::tangent | shape_options::bitangent;
  const bool generate_wireframe = testMaskBit(options, shape_options::wireframe);
  const bool generate_normals = testMaskBit(options, shape_options::normal);
  bool generate_uvs = testMaskBit(options, shape_options::uv);
  const bool generate_tangents = testMaskBit(options, shape_options::tangent);
  const bool generate_bitangents = testMaskBit(options, shape_options::bitangent);

  Model model;

  const u64 position_id = model.pushAttribute<point2>("position");
  const u64 uv_id = generate_uvs ? model.pushAttribute<point2>("uvs") : 0;

  std::vector<i32> indices;

  if (testMaskBit(options, shape_options::unique_positions)) {
    // TODO
  } else {
    model.resize(4);
    model.attributeValue<point2>(position_id, 0) = {box.lower.x, box.lower.y};
    model.attributeValue<point2>(position_id, 1) = {box.upper.x, box.lower.y};
    model.attributeValue<point2>(position_id, 2) = {box.upper.x, box.upper.y};
    model.attributeValue<point2>(position_id, 3) = {box.lower.x, box.upper.y};
    if (generate_uvs) {
      model.attributeValue<point2>(uv_id, 0) = {0, 0};
      model.attributeValue<point2>(uv_id, 1) = {1, 0};
      model.attributeValue<point2>(uv_id, 2) = {1, 1};
      model.attributeValue<point2>(uv_id, 3) = {0, 1};
    }
    if (generate_wireframe) {
      model.setPrimitiveType(GeometricPrimitiveType::LINES);
      indices = {0, 1, 1, 2, 2, 3, 3, 0};
    } else {
      model.setPrimitiveType(GeometricPrimitiveType::TRIANGLES);
      indices = {0, 1, 2, 0, 2, 3};
    };
  }
  model = indices;
  return model;
}

Model Shapes::segment(const Segment3 &s, shape_options options) {
  Model model;
  model.pushAttribute<point3>("position");
  if (testMaskBit(options, shape_options::uv))
    model.pushAttribute<point2>("uv");
  model.resize(2);
  model.attributeValue<point3>(0, 0) = s.a;
  model.attributeValue<point3>(0, 1) = s.b;
  if (testMaskBit(options, shape_options::uv)) {
    model.attributeValue<point2>(1, 0) = {0.f, 0.f};
    model.attributeValue<point2>(1, 1) = {1.f, 1.f};
  }
  std::vector<i32> indices = {0, 1};
  model = indices;
  model.setPrimitiveType(GeometricPrimitiveType::LINES);
  return model;
}

}