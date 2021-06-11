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
/// \file vk_texture_image.h
/// \author FilipeCN (filipedecn@gmail.com)
/// \date 2019-12-09
///
/// \brief

#ifndef CIRCE_VK_TEXTURE_IMAGE_H
#define CIRCE_VK_TEXTURE_IMAGE_H

#include <circe/vk/storage/device_memory.h>
#include <string>
#include <memory>

namespace circe::vk {

class Texture {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  Texture();
  ///
  /// \param logical_device
  /// \param filename
  /// \param queue_family_index
  /// \param queue
  explicit Texture(const LogicalDevice::Ref &logical_device,
                   const std::string &filename, uint32_t queue_family_index,
                   VkQueue queue);
  /// \param logical_device **[in]** logical device (on which the image
  /// will be created)
  /// \param type **[in]** number of dimensions of the image
  /// \param format **[in]** number of components and number of bits of a
  /// texel in the image
  /// \param size **[in]** image's dimensions (in texels)
  /// \param num_mipmaps **[in]** number of mipmap levels
  /// \param num_layers **[in]** number of layers
  /// \param samples **[in]** number of samples
  /// \param usage_scenarios **[in]**
  /// \param cubemap **[in]**
  Texture(const LogicalDevice::Ref &logical_device, VkImageType type,
          VkFormat format, VkExtent3D size, uint32_t num_mipmaps,
          uint32_t num_layers, VkSampleCountFlagBits samples,
          VkImageUsageFlags usage_scenarios, bool cubemap);
  Texture(const Texture& other) = delete;
  Texture(Texture&& other) noexcept;
  virtual ~Texture();
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  Texture &operator=(const Texture &other) = delete;
  Texture &operator=(Texture &&other) noexcept;
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  void destroy();
  [[nodiscard]] bool good() const;
  void setData(const unsigned char *data, uint32_t queue_family_index,
               VkQueue queue);
  // ***********************************************************************
  //                           FIELDS
  // ***********************************************************************
  [[nodiscard]] const Image *image() const;

private:
  ///\param queue **[in]**
  ///\param queue_family_index **[in]**
  void generateMipmaps(VkQueue queue, uint32_t queue_family_index);
  LogicalDevice::Ref logical_device_;
  Image image_;
  DeviceMemory image_memory_;
};

} // namespace circe::vk

#endif
