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
///\file model.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2020-04-19
///
///\brief

#include <circe/vk/scene/scene_model.h>
#include <circe/vk/storage/buffer.h>
#include <circe/vk/pipeline/command_buffer.h>
#include <ponos/geometry/vector.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <unordered_map>

struct Vertex {
  ponos::vec3 pos;
  ponos::vec3 color;
  ponos::vec2 tex_coord;
  ponos::vec3 normal;
  ponos::vec3 tangent;
  ponos::vec3 bi_tangent;

  bool operator==(const Vertex &other) const {
    return pos == other.pos && color == other.color &&
        tex_coord == other.tex_coord;
  }
};

namespace std {
template<> struct hash<Vertex> {
  size_t operator()(Vertex const &vertex) const {
    return ((hash<ponos::vec3>()(vertex.pos) ^
        (hash<ponos::vec3>()(vertex.color) << 1)) >>
                                                  1) ^
        (hash<ponos::vec2>()(vertex.tex_coord) << 1);
  }
};
} // namespace std

namespace circe::vk {

Model::Model() = default;

Model::Model(const LogicalDevice::Ref &device, VkQueue copy_queue, uint32_t queue_family_index) : device_{device} {
  setDevice(device);
  setDeviceQueue(copy_queue, queue_family_index);
}

void Model::setDevice(const LogicalDevice::Ref &device) {
  device_ = device;
  vertices_m_.setDevice(device);
  indices_m_.setDevice(device);
}

void Model::setDeviceQueue(VkQueue queue, uint32_t family_index) {
  queue_ = queue;
  family_index_ = family_index;
}

void addVertex(std::vector<float> &vertices, const VertexLayout &layout,
               const Vertex &v, const ponos::vec2 &uv_scale,
               const ponos::vec3 &scale, const ponos::vec3 &center) {
  for (auto &component : layout.components) {
    switch (component) {
    case VERTEX_COMPONENT_POSITION:vertices.push_back(v.pos.x * scale.x + center.x);
      vertices.push_back(-v.pos.y * scale.y + center.y);
      vertices.push_back(v.pos.z * scale.z + center.z);
      break;
    case VERTEX_COMPONENT_NORMAL:vertices.push_back(v.normal.x);
      vertices.push_back(-v.normal.y);
      vertices.push_back(v.normal.z);
      break;
    case VERTEX_COMPONENT_UV:vertices.push_back(v.tex_coord.x * uv_scale.x);
      vertices.push_back(v.tex_coord.y * uv_scale.y);
      break;
    case VERTEX_COMPONENT_COLOR:vertices.push_back(v.color.x);
      vertices.push_back(v.color.y);
      vertices.push_back(v.color.z);
      break;
    case VERTEX_COMPONENT_TANGENT:vertices.push_back(v.tangent.x);
      vertices.push_back(v.tangent.y);
      vertices.push_back(v.tangent.z);
      break;
    case VERTEX_COMPONENT_BITANGENT:vertices.push_back(v.bi_tangent.x);
      vertices.push_back(v.bi_tangent.y);
      vertices.push_back(v.bi_tangent.z);
      break;
      // Dummy components for padding
    case VERTEX_COMPONENT_DUMMY_FLOAT:vertices.push_back(0.0f);
      break;
    case VERTEX_COMPONENT_DUMMY_VEC4:vertices.push_back(0.0f);
      vertices.push_back(0.0f);
      vertices.push_back(0.0f);
      vertices.push_back(0.0f);
      break;
    }
  }
}

bool Model::loadFromOBJ(const std::string &obj_filename, VertexLayout layout) {
  ponos::vec2 uv_scale(1.f);
  ponos::vec3 scale(1.f);
  ponos::vec3 center(0.f);
  // host data
  std::vector<float> h_vertices;
  std::vector<uint32_t> h_indices;
  // use tiny obj to load host data
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        obj_filename.c_str()))
    throw std::runtime_error(warn + err);

  std::unordered_map<Vertex, uint32_t> unique_vertices = {};
  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {

      Vertex vertex = {};

      vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                    -attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};

      vertex.tex_coord = {attrib.texcoords[2 * index.texcoord_index + 0],
                          1.0f -
                              attrib.texcoords[2 * index.texcoord_index + 1]};

      vertex.color = {1.0f, 1.0f, 1.0f};

      if (unique_vertices.count(vertex) == 0) {
        unique_vertices[vertex] =
            static_cast<uint32_t>(h_vertices.size() / (layout.stride() / sizeof(float)));
        addVertex(h_vertices, layout, vertex, uv_scale, scale, center);
      }

      h_indices.push_back(unique_vertices[vertex]);
    }
  }
  return loadFromData(h_vertices, h_indices);
}

bool Model::loadFromData(const std::vector<float> &vertices,
                         const std::vector<uint32_t> &indices) {
  // Use staging buffer to move vertex and index buffer to device local memory
  uint32_t vertex_buffer_size = vertices.size() * sizeof(float);
  uint32_t index_buffer_size = indices.size() * sizeof(uint32_t);
  // Create a staging buffer for vertices and copy data to it
  Buffer staging_v(device_, vertex_buffer_size,
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  DeviceMemory staging_v_m(staging_v, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  staging_v_m.bind(staging_v);
  staging_v_m.copy(vertices.data(), staging_v.size());

  // Create a staging buffer for indices and copy data to it
  Buffer staging_i(device_, index_buffer_size,
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  DeviceMemory staging_i_m(staging_i, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  staging_i_m.bind(staging_i);
  staging_i_m.copy(indices.data(), staging_i.size());

  // init device local buffers
  vertices_.init(device_, vertices.size() * sizeof(float),
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  VkMemoryRequirements memory_requirements{};
  if (!vertices_.memoryRequirements(memory_requirements))
    return false;
  vertices_m_.allocate(memory_requirements,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vertices_m_.bind(vertices_);

  indices_.init(device_, indices.size() * sizeof(uint32_t),
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  if (!indices_.memoryRequirements(memory_requirements))
    return false;
  indices_m_.allocate(memory_requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  indices_m_.bind(indices_);

  CommandPool::submitCommandBuffer(
      device_, family_index_, queue_, [&](circe::vk::CommandBuffer &cb) {
        cb.copy(staging_v, 0, vertices_, 0, vertex_buffer_size);
        cb.copy(staging_i, 0, indices_, 0, index_buffer_size);
      });

  return true;
}

const Buffer &Model::vertices() const { return vertices_; }

const Buffer &Model::indices() const { return indices_; }

} // namespace circe