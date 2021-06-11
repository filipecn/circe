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
/// \file vk_sampler.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date 2019-12-10
///
/// \brief

#ifndef CIRCE_VK_SAMPLER_H
#define CIRCE_VK_SAMPLER_H

#include <circe/vk/core/logical_device.h>

namespace circe::vk {

class Sampler {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  Sampler();
  Sampler(const LogicalDevice::Ref &logical_device,
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
          VkBool32 unnormalized_coordinates);
  ~Sampler();
  Sampler(const Sampler &other) = delete;
  Sampler(Sampler &&other) noexcept;
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  Sampler &operator=(const Sampler &other) = delete;
  Sampler &operator=(Sampler &&other) noexcept;
  // ***********************************************************************
  //                           CREATION
  // ***********************************************************************
  ///
  /// \param logical_device
  /// \param mag_filter
  /// \param min_filter
  /// \param mipmap_mode
  /// \param address_mode_u
  /// \param address_mode_v
  /// \param address_mode_w
  /// \param mip_lod_bias
  /// \param anisotropy_enable
  /// \param max_anisotropy
  /// \param compare_enable
  /// \param compare_op
  /// \param min_lod
  /// \param max_lod
  /// \param border_color
  /// \param unnormalized_coordinates
  /// \return
  bool init(const LogicalDevice::Ref &logical_device,
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
            VkBool32 unnormalized_coordinates);
  void destroy();
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  [[nodiscard]] VkSampler handle() const;

private:
  LogicalDevice::Ref logical_device_;
  VkSampler vk_sampler_ = VK_NULL_HANDLE;
};

} // circe::vk namespace

#endif