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
/// \file vk_sampler.cpp
/// \author FilipeCN (filipedecn@gmail.com)
/// \date 2019-12-10
///
/// \brief

#include <circe/vk/texture/sampler.h>
#include <circe/vk/utils/vk_debug.h>

namespace circe::vk {

Sampler::Sampler() = default;

Sampler::Sampler(const LogicalDevice::Ref &logical_device,
                 VkFilter mag_filter,
                 VkFilter min_filter,
                 VkSamplerMipmapMode mipmap_mode,
                 VkSamplerAddressMode address_mode_u,
                 VkSamplerAddressMode address_mode_v,
                 VkSamplerAddressMode address_mode_w,
                 float mip_lod_bias,
                 VkBool32 anisotropy_enable,
                 float max_anisotropy,
                 VkBool32 compare_enable,
                 VkCompareOp compare_op,
                 float min_lod,
                 float max_lod,
                 VkBorderColor border_color,
                 VkBool32 unnormalized_coordinates) : logical_device_(
    logical_device) {
  PONOS_CHECK_EXP_WITH_LOG(init(logical_device,
                                mag_filter,
                                min_filter,
                                mipmap_mode,
                                address_mode_u,
                                address_mode_v,
                                address_mode_w,
                                mip_lod_bias,
                                anisotropy_enable,
                                max_anisotropy,
                                compare_enable,
                                compare_op,
                                min_lod,
                                max_lod,
                                border_color,
                                unnormalized_coordinates), "unable to create sampler")
}

Sampler::Sampler(Sampler &&other) noexcept {
  *this = std::move(other);
}

Sampler::~Sampler() {
  destroy();
}

Sampler &Sampler::operator=(Sampler &&other) noexcept {
  destroy();
  logical_device_ = other.logical_device_;
  vk_sampler_ = other.vk_sampler_;
  other.vk_sampler_ = VK_NULL_HANDLE;
  return *this;
}

bool Sampler::init(const LogicalDevice::Ref &logical_device,
                   VkFilter mag_filter,
                   VkFilter min_filter,
                   VkSamplerMipmapMode mipmap_mode,
                   VkSamplerAddressMode address_mode_u,
                   VkSamplerAddressMode address_mode_v,
                   VkSamplerAddressMode address_mode_w,
                   float mip_lod_bias,
                   VkBool32 anisotropy_enable,
                   float max_anisotropy,
                   VkBool32 compare_enable,
                   VkCompareOp compare_op,
                   float min_lod,
                   float max_lod,
                   VkBorderColor border_color,
                   VkBool32 unnormalized_coordinates) {
  destroy();
  logical_device_ = logical_device;
  VkSamplerCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.magFilter = mag_filter;
  info.minFilter = min_filter;
  info.mipmapMode = mipmap_mode;
  info.addressModeU = address_mode_u;
  info.addressModeV = address_mode_v;
  info.addressModeW = address_mode_w;
  info.mipLodBias = mip_lod_bias;
  info.anisotropyEnable = anisotropy_enable;
  info.maxAnisotropy = max_anisotropy;
  info.compareEnable = compare_enable;
  info.compareOp = compare_op;
  info.minLod = min_lod;
  info.maxLod = max_lod;
  info.borderColor = border_color;
  info.unnormalizedCoordinates = unnormalized_coordinates;

  R_CHECK_VULKAN(vkCreateSampler(logical_device_.handle(),
                                 &info,
                                 nullptr,
                                 &vk_sampler_), false)
  return true;
}

void Sampler::destroy() {
  if (vk_sampler_ != VK_NULL_HANDLE) {
    vkDestroySampler(logical_device_.handle(), vk_sampler_, nullptr);
    vk_sampler_ = VK_NULL_HANDLE;
  }
}

VkSampler Sampler::handle() const {
  return vk_sampler_;
}

} // circe::vk namespace
