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
///\file vk_shader_module.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-11-01
///
///\brief

#include <circe/vk/pipeline/shader_module.h>
#include <circe/vk/utils/vk_debug.h>
#include <fstream>

namespace circe::vk {

ShaderModule::ShaderModule() = default;

ShaderModule::ShaderModule(const LogicalDevice::Ref &logical_device,
                           const hermes::Path &filename)
    : logical_device_(logical_device) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  std::vector<u8> source_code = hermes::FileSystem::readBinaryFile(filename.fullName().c_str());
  if (source_code.empty()) {
    HERMES_LOG_WARNING(hermes::Str::concat("Could not read shader file:", filename))
    return;
  }
  VkShaderModuleCreateInfo shader_module_create_info = {
      VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, // VkStructureType sType
      nullptr,                                     // const void * pNext
      0,                              // VkShaderModuleCreateFlags    flags
      source_code.size(), // size_t                       codeSize
      reinterpret_cast<uint32_t const *>(
          source_code.data()) // const uint32_t             * pCode
  };
  CHECK_VULKAN(vkCreateShaderModule(logical_device.handle(),
                                    &shader_module_create_info, nullptr,
                                    &vk_shader_module_))
}

ShaderModule::ShaderModule(const LogicalDevice::Ref &logical_device,
                           std::vector<char> const &source_code)
    : logical_device_(logical_device) {
  HERMES_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  VkShaderModuleCreateInfo shader_module_create_info = {
      VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, // VkStructureType sType
      nullptr,            // const void                 * pNext
      0,                  // VkShaderModuleCreateFlags    flags
      source_code.size(), // size_t                       codeSize
      reinterpret_cast<uint32_t const *>(
          source_code.data()) // const uint32_t             * pCode
  };
  CHECK_VULKAN(vkCreateShaderModule(logical_device.handle(),
                                    &shader_module_create_info, nullptr,
                                    &vk_shader_module_))
}

ShaderModule::~ShaderModule() {
  destroy();
}

ShaderModule &ShaderModule::operator=(ShaderModule &&other) noexcept {
  destroy();
  logical_device_ = other.logical_device_;
  vk_shader_module_ = other.vk_shader_module_;
  return *this;
}

void ShaderModule::setDevice(const LogicalDevice::Ref &logical_device) {
  logical_device_ = logical_device;
}

void ShaderModule::destroy() {
  if (VK_NULL_HANDLE != vk_shader_module_)
    vkDestroyShaderModule(logical_device_.handle(), vk_shader_module_,
                          nullptr);
  vk_shader_module_ = VK_NULL_HANDLE;
}

bool ShaderModule::load(const std::string &filename) {
  if (!logical_device_.good())
    return false;
  std::vector<u8> source_code = hermes::FileSystem::readBinaryFile(filename.c_str());
  HERMES_LOG_AND_RETURN_VALUE_IF_NOT(!source_code.empty(), false,
                               hermes::Str::concat("Could not read shader file:", filename))
  VkShaderModuleCreateInfo shader_module_create_info = {
      VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, // VkStructureType sType
      nullptr,            // const void                 * pNext
      0,                  // VkShaderModuleCreateFlags    flags
      source_code.size(), // size_t                       codeSize
      reinterpret_cast<uint32_t const *>(
          source_code.data()) // const uint32_t             * pCode
  };
  CHECK_VULKAN(vkCreateShaderModule(logical_device_.handle(),
                                    &shader_module_create_info, nullptr,
                                    &vk_shader_module_))
  return true;
}

VkShaderModule ShaderModule::handle() const { return vk_shader_module_; }

bool ShaderModule::good() const {
  return logical_device_.good() && vk_shader_module_;
}

} // namespace circe
