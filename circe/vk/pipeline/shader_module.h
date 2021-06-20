/// Copyright (c) 2019, FilipeCN.
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
///\file vk_shader_module.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-11-01
///
///\brief

#ifndef CIRCE_VULKAN_SHADER_MODULE_H
#define CIRCE_VULKAN_SHADER_MODULE_H

#include <circe/vk/core/logical_device.h>
#include <ponos/common/file_system.h>

namespace circe::vk {

/// This class contains the information required to specialize a shader, which
/// is the process of building a shader with some of its constants compiled in.
class ShaderSpecializationInfo {
public:
  ShaderSpecializationInfo();
  ~ShaderSpecializationInfo();

private:
  std::vector<VkSpecializationMapEntry> entries_;
};

// The computations executed inside the pipeline are performed by shaders.
// Shaders are represented by Shader Modules and must be provided to Vulkan as
// SPIR-V assembly code. A single module may contain code for multiple shader
// stages.
class ShaderModule {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  ShaderModule();
  ShaderModule(const LogicalDevice::Ref &logical_device,
               const ponos::Path &filename);
  ShaderModule(const LogicalDevice::Ref &logical_device,
               std::vector<char> const &source_code);
  ~ShaderModule();
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  ShaderModule &operator=(const ShaderModule &other) = delete;
  ShaderModule &operator=(ShaderModule &&other) noexcept;
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  void destroy();
  [[nodiscard]] bool good() const;
  void setDevice(const LogicalDevice::Ref &logical_device);
  bool load(const std::string& filename);
  [[nodiscard]] VkShaderModule handle() const;

private:
  LogicalDevice::Ref logical_device_;
  VkShaderModule vk_shader_module_ = VK_NULL_HANDLE;
};

} // namespace circe

#endif // CIRCE_VULKAN_SHADER_MODULE_H