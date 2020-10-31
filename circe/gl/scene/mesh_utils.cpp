/*
 * Copyright (c) 2017 FilipeCN
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <circe/gl/scene/mesh_utils.h>

namespace circe::gl {

ponos::RawMesh *create_grid_mesh(const ponos::size3 &d, float s,
                                 const ponos::vec3 &o) {
  auto *m = new ponos::RawMesh();
  // XY
  for (auto ij : ponos::Index2Range<i32>(d[0], d[1])) {
    ponos::point3 p(ij[0], ij[1], 0.f);
    p = p * s + o;
    m->positions.emplace_back(p.x);
    m->positions.emplace_back(p.y);
    m->positions.emplace_back(p.z);
  }
  for (auto ij : ponos::Index2Range<i32>(d[0], d[1])) {
    ponos::point3 p(ij[0], ij[1], d[2] - 1);
    p = p * s + o;
    m->positions.emplace_back(p.x);
    m->positions.emplace_back(p.y);
    m->positions.emplace_back(p.z);
  }
  // YZ
  for (auto ij : ponos::Index2Range<i32>(d[1], d[2])) {
    ponos::point3 p(0.f, ij[0], ij[1]);
    p = p * s + o;
    m->positions.emplace_back(p.x);
    m->positions.emplace_back(p.y);
    m->positions.emplace_back(p.z);
  }
  for (auto ij : ponos::Index2Range<i32>(d[1], d[2])) {
    ponos::point3 p(d[0] - 1, ij[0], ij[1]);
    p = p * s + o;
    m->positions.emplace_back(p.x);
    m->positions.emplace_back(p.y);
    m->positions.emplace_back(p.z);
  }
  // XZ
  for (auto ij : ponos::Index2Range<i32>(d[0], d[2])) {
    ponos::point3 p(ij[0], 0.f, ij[1]);
    p = p * s + o;
    m->positions.emplace_back(p.x);
    m->positions.emplace_back(p.y);
    m->positions.emplace_back(p.z);
  }
  for (auto ij : ponos::Index2Range<i32>(d[0], d[2])) {
    ponos::point3 p(ij[0], d[1] - 1, ij[1]);
    p = p * s + o;
    m->positions.emplace_back(p.x);
    m->positions.emplace_back(p.y);
    m->positions.emplace_back(p.z);
  }
  m->positionDescriptor.count = m->positions.size() / 3;
  int xy = d[0] * d[1];
  // create indices for xy planes
  for (auto ij : ponos::Index2Range<i32>(d[0], d[1])) {
    m->positionsIndices.emplace_back(ij[0] * d[0] + ij[1]);
    m->positionsIndices.emplace_back(xy + ij[0] * d[0] + ij[1]);
  }
  int acc = xy * 2;
  int yz = d[1] * d[2];
  // create indices for yz planes
  for (auto ij : ponos::Index2Range<i32>(d[1], d[2])) {
    m->positionsIndices.emplace_back(acc + ij[0] * d[1] + ij[1]);
    m->positionsIndices.emplace_back(acc + yz + ij[0] * d[1] + ij[1]);
  }
  acc += yz * 2;
  int xz = d[0] * d[2];
  // create indices for xz planes
  for (auto ij : ponos::Index2Range<i32>(d[0], d[2])) {
    m->positionsIndices.emplace_back(acc + ij[0] * d[1] + ij[1]);
    m->positionsIndices.emplace_back(acc + xz + ij[0] * d[1] + ij[1]);
  }
  return m;
}

ponos::RawMesh *create_wireframe_mesh(const ponos::RawMesh *m) {
  auto *mesh = new ponos::RawMesh();
  mesh->positionDescriptor.count = m->positionDescriptor.count;
  mesh->positions = std::vector<float>(m->positions);
  size_t nelements = m->indices.size() / m->meshDescriptor.elementSize;
  for (size_t i = 0; i < nelements; i++)
    for (size_t j = 0; j < m->meshDescriptor.elementSize; j++) {
      mesh->positionsIndices.emplace_back(
          m->indices[i * m->meshDescriptor.elementSize + j].positionIndex);
      mesh->positionsIndices.emplace_back(
          m->indices[i * m->meshDescriptor.elementSize +
                     (j + 1) % m->meshDescriptor.elementSize]
              .positionIndex);
    }
  return mesh;
}
/*
ponos::RawMesh *create_icosphere_mesh(const ponos::point3 &center,
                                      float radius,
                                      size_t divisions,
                                      bool generateNormals,
                                      bool generateUVs) {
  auto *mesh = new ponos::RawMesh();
  // icosahedron
  float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
  mesh->addVertex({-1, t, 0});
  mesh->addVertex({1, t, 0});
  mesh->addVertex({-1, -t, 0});
  mesh->addVertex({1, -t, 0});
  mesh->addVertex({0, -1, t});
  mesh->addVertex({0, 1, t});
  mesh->addVertex({0, -1, -t});
  mesh->addVertex({0, 1, -t});
  mesh->addVertex({t, 0, -1});
  mesh->addVertex({t, 0, 1});
  mesh->addVertex({-t, 0, -1});
  mesh->addVertex({-t, 0, 1});
  mesh->addFace({0, 11, 5});
  mesh->addFace({0, 5, 1});
  mesh->addFace({0, 1, 7});
  mesh->addFace({0, 7, 10});
  mesh->addFace({0, 10, 11});
  mesh->addFace({1, 5, 9});
  mesh->addFace({5, 11, 4});
  mesh->addFace({11, 10, 2});
  mesh->addFace({10, 7, 6});
  mesh->addFace({7, 1, 8});
  mesh->addFace({3, 9, 4});
  mesh->addFace({3, 4, 2});
  mesh->addFace({3, 2, 6});
  mesh->addFace({3, 6, 8});
  mesh->addFace({3, 8, 9});
  mesh->addFace({4, 9, 5});
  mesh->addFace({2, 4, 11});
  mesh->addFace({6, 2, 10});
  mesh->addFace({8, 6, 7});
  mesh->addFace({9, 8, 1});
  std::map<std::pair<int, int>, size_t> indicesCache;
  std::function<size_t(int, int)> midpoint = [&](int a, int b) -> size_t {
    std::pair<int, int> key(std::min(a, b), std::max(a, b));
    size_t n = mesh->vertices.size() / 3;
    if (indicesCache.find(key) != indicesCache.end())
      return indicesCache[key];
    ponos::point3 pa(&mesh->vertices[3 * a]);
    ponos::point3 pb(&mesh->vertices[3 * b]);
    auto pm = pa + (pb - pa) * 0.5f;
    mesh->addVertex({pm.x, pm.y, pm.z});
    return n;
  };
  for (size_t i = 0; i < divisions; i++) {
    size_t n = mesh->indices.size() / 3;
    for (size_t j = 0; j < n; j++) {
      int v0 = mesh->indices[j * 3 + 0].vertexIndex;
      int v1 = mesh->indices[j * 3 + 1].vertexIndex;
      int v2 = mesh->indices[j * 3 + 2].vertexIndex;
      int a = midpoint(v0, v1);
      int b = midpoint(v1, v2);
      int c = midpoint(v2, v0);
      mesh->addFace({v0, a, c});
      mesh->addFace({v1, b, a});
      mesh->addFace({v2, c, b});
      mesh->addFace({a, c, b});
    }
  }
  // normalize positions
  size_t n = mesh->vertices.size() / 3;
  for (size_t i = 0; i < n; i++) {
    ponos::Vector3 v(&mesh->vertices[i * 3]);
    v = ponos::normalize(v);
    mesh->vertices[i * 3 + 0] = v.x;
    mesh->vertices[i * 3 + 1] = v.y;
    mesh->vertices[i * 3 + 2] = v.z;
  }
  // fix normal and uvs indices
  for (auto &i : mesh->indices)
    i.normalIndex = i.texcoordIndex = i.vertexIndex;
  size_t vertexCount = mesh->vertices.size() / 3;
  if(generateNormals) {
    mesh->normals = mesh->vertices;
    mesh->normalDescriptor.count = vertexCount;
    mesh->normalDescriptor.elementSize = 3;
  }
  if(generateUVs) {
    std::function<ponos::point2(float, float, float)> parametric = [](float x,
float y, float z) -> ponos::point2 {
      return ponos::point2(std::atan2(y, x), std::acos(z));
    };
    for (size_t i = 0; i < vertexCount; i++) {
      auto uv = parametric(mesh->vertices[i * 3 + 0], mesh->vertices[i * 3 +
1], mesh->vertices[i * 3 + 2]); mesh->addUV({uv.x, uv.y});
    }
    mesh->texcoordDescriptor.count = vertexCount;
    mesh->texcoordDescriptor.elementSize = 2;
  }
  // describe mesh
  mesh->primitiveType = ponos::GeometricPrimitiveType::TRIANGLES;
  mesh->meshDescriptor.count = mesh->indices.size() / 3;
  mesh->meshDescriptor.elementSize = 3;
  mesh->vertexDescriptor.count = vertexCount;
  mesh->vertexDescriptor.elementSize = 3;
  mesh->apply(ponos::scale(radius, radius, radius) *
      ponos::translate(ponos::Vector3(center)));
  mesh->computeBBox();
  mesh->splitIndexData();
  mesh->buildInterleavedData();
  return mesh;
}
*/
} // namespace circe
