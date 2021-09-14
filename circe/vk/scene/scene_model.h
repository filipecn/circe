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
///\date 2020-04-19
///
/// Based on
/// https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanModel.hpp
///\brief

#ifndef CIRCE_VK_SCENE_MODEL_H
#define CIRCE_VK_SCENE_MODEL_H

#include <circe/vk/storage/device_memory.h>
#include <hermes/common/defs.h>

namespace circe::vk {

/** @brief Vertex layout components */
typedef enum VertexComponent {
  VERTEX_COMPONENT_POSITION = 0x0,
  VERTEX_COMPONENT_NORMAL = 0x1,
  VERTEX_COMPONENT_COLOR = 0x2,
  VERTEX_COMPONENT_UV = 0x3,
  VERTEX_COMPONENT_TANGENT = 0x4,
  VERTEX_COMPONENT_BITANGENT = 0x5,
  VERTEX_COMPONENT_DUMMY_FLOAT = 0x6,
  VERTEX_COMPONENT_DUMMY_VEC4 = 0x7
} Component;

/** @brief Stores vertex layout components for model loading, Vulkan vertex
 * input and attribute bindings  */
struct VertexLayout {
public:
  /** @brief Components used to generate vertices from */
  std::vector<VertexComponent> components;
  std::vector<VkFormat> formats;
  u32 sizes[8] = {
      3 * sizeof(float), // VERTEX_COMPONENT_POSITION = 0x0,
      3 * sizeof(float), // VERTEX_COMPONENT_NORMAL = 0x1,
      3 * sizeof(float), // VERTEX_COMPONENT_COLOR = 0x2,
      2 * sizeof(float), // VERTEX_COMPONENT_UV = 0x3,
      3 * sizeof(float), // VERTEX_COMPONENT_TANGENT = 0x4,
      3 * sizeof(float), // VERTEX_COMPONENT_BITANGENT = 0x5,
      1 * sizeof(float), // VERTEX_COMPONENT_DUMMY_FLOAT = 0x6,
      4 * sizeof(float) // VERTEX_COMPONENT_DUMMY_VEC4 = 0x7
  };
  VkFormat default_formats[8] = {
      VK_FORMAT_R32G32B32_SFLOAT, /// VERTEX_COMPONENT_POSITION = 0x0,
      VK_FORMAT_R32G32B32_SFLOAT, /// VERTEX_COMPONENT_NORMAL = 0x1,
      VK_FORMAT_R32G32B32_SFLOAT, /// VERTEX_COMPONENT_COLOR = 0x2,
      VK_FORMAT_R32G32_SFLOAT, /// VERTEX_COMPONENT_UV = 0x3,
      VK_FORMAT_R32G32B32_SFLOAT, /// VERTEX_COMPONENT_TANGENT = 0x4,
      VK_FORMAT_R32G32B32_SFLOAT, /// VERTEX_COMPONENT_BITANGENT = 0x5,
      VK_FORMAT_R32_SFLOAT, /// VERTEX_COMPONENT_DUMMY_FLOAT = 0x6,
      VK_FORMAT_R32G32B32A32_SFLOAT, /// VERTEX_COMPONENT_DUMMY_VEC4 = 0x7
  };
  VertexLayout() = default;
  ///
  /// \param components
  /// \param formats
  explicit VertexLayout(std::vector<VertexComponent> components,
                        std::vector<VkFormat> formats = std::vector<VkFormat>()) {
    this->components = std::move(components);
    this->formats = std::move(formats);
    if (!this->components.empty() && this->formats.empty())
      // if not formats are provided, fill with default ones
      fillWithDefaultFormats();
  }
  void fillWithDefaultFormats() {
    this->formats.clear();
    for (auto &component : this->components)
      this->formats.emplace_back(default_formats[component]);
  }
  VertexComponent operator[](u32 i) const { return components[i]; };
  VkFormat componentFormat(VertexComponent component) {
    for (size_t i = 0; i < components.size(); ++i)
      if (components[i] == component)
        return formats[i];
    std::cerr << "Invalid Vertex Layout Component!\n";
    return VK_FORMAT_R32G32B32_SFLOAT;
  }
  u32 componentOffset(VertexComponent component) {
    u32 o = 0;
    for (auto &c : components) {
      if (c == component)
        return o;
      o += sizes[c];
    }
    std::cerr << "Invalid Vertex Layout Component!\n";
    return o;
  }
  u32 stride() {
    u32 res = 0;
    for (auto &component : components)
      res += sizes[component];
    return res;
  }
};

/// Holds data of a set of scene meshes, containing buffers for vertices,
/// texture coordinates, normals, etc. A model can be composed of multiple
/// shapes, each representing a single mesh.
class Model {
public:
  struct Shape {
    u32 vertex_base;
    u32 vertex_count;
    u32 index_base;
    u32 index_count;
  };
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  /// Default constructor
  Model();
  Model(const LogicalDevice::Ref &device, VkQueue copy_queue, u32 queue_family_index);
  void setDevice(const LogicalDevice::Ref &device);
  void setDeviceQueue(VkQueue queue, u32 family_index);
  ///\brief
  ///\param obj_filename **[in]**
  ///\param layout **[in]**
  ///\return bool
  bool loadFromOBJ(const std::string &obj_filename, VertexLayout layout);
  ///\brief
  ///
  ///\param vertices **[in]**
  ///\param indices **[in]**
  ///\return bool
  bool loadFromData(const std::vector<float> &vertices,
                    const std::vector<u32> &indices);
  // ***********************************************************************
  //                            FIELDS
  // ***********************************************************************
  [[nodiscard]] const Buffer &vertices() const;
  [[nodiscard]] const Buffer &indices() const;

private:
  LogicalDevice::Ref device_;
  Buffer vertices_, indices_;
  DeviceMemory vertices_m_, indices_m_;
  std::vector<Shape> shapes_;
  VkQueue queue_{nullptr};
  u32 family_index_{0};
};

} // namespace circe

#endif
