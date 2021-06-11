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
///\file vk_renderpass.h
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-11-06
///
///\brief

#ifndef CIRCE_VULKAN_RENDERPASS_H
#define CIRCE_VULKAN_RENDERPASS_H

#include <circe/vk/storage/image.h>

namespace circe::vk {

/// A pass within the renderpass is called a subpass. Each subpass references a
/// number of attachments (from the order added on the RenderPass object) as
/// input and outputs.
class SubpassDescription {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  SubpassDescription();
  ~SubpassDescription() = default;
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  ///\brief Adds an input attachment reference
  /// Input attachments are attachments from which the subpass can read from.
  ///\param attachment **[in]** index into the array of attachments created in
  /// the RenderPass object
  ///\param layout **[in]** image layout that the attachment is expected to be
  /// in at this subpass
  ///\return uint32_t index of the added attachment reference
  uint32_t addInputAttachmentRef(uint32_t attachment, VkImageLayout layout);
  ///\brief Adds an color attachment reference
  /// Color attachments are attachments from which the subpass write to.
  /// Resolve attachments are attachments that correspond to color attachments,
  /// and are used to resove multisample image data
  ///\param attachment **[in]** index into the array of attachments created in
  /// the RenderPass object
  ///\param layout **[in]** image layout that the attachment is expected to be
  /// in at this subpass
  ///\return uint32_t index of the added attachment reference
  uint32_t addColorAttachmentRef(uint32_t attachment, VkImageLayout layout);
  ///\param resolve_attachment **[in]** index into the array of attachments
  /// created in the RenderPass object
  ///\param resolve_layout **[in]** image layout that the attachment is expected
  /// to be in at this subpass
  ///\return uint32_t index of the added attachment reference
  uint32_t addResolveAttachmentRef(uint32_t resolve_attachment,
                                   VkImageLayout resolve_layout);
  ///\brief Sets the depth-stencil attachment reference
  /// The depth/stencil attachment is the attachment used as a depth and stencil
  /// buffer.
  ///\param attachment **[in]** index into the array of attachments created in
  /// the RenderPass object
  ///\param layout **[in]** image layout that the attachment is expected to be
  /// in at this subpass
  void setDepthStencilAttachmentRef(uint32_t attachment, VkImageLayout layout);
  /// Preserved attachments live across the subpass and prevent Vulkan from
  /// making any optimizations that might disturb its contents.
  ///\param attachment **[in]** index into the array of attachments created in
  /// the RenderPass object
  void preserveAttachment(uint32_t attachment);
  // ***********************************************************************
  //                           FIELDS
  // ***********************************************************************
  [[nodiscard]] const VkAttachmentReference &depthStencilAttachmentRef() const;
  [[nodiscard]] const std::vector<VkAttachmentReference> &inputAttachmentRefs() const;
  [[nodiscard]] const std::vector<VkAttachmentReference> &colorAttachmentRefs() const;
  [[nodiscard]] const std::vector<VkAttachmentReference> &resolveAttachmentRefs() const;
  [[nodiscard]] const std::vector<uint32_t> &preserveAttachmentRefs() const;
  [[nodiscard]] bool hasDepthStencilAttachmentRef() const;

private:
  bool depth_stencil_attachment_set_ = false;
  VkAttachmentReference vk_depth_stencil_attachment_{};
  std::vector<VkAttachmentReference> vk_input_attachments_;
  std::vector<VkAttachmentReference> vk_color_attachments_;
  std::vector<VkAttachmentReference> vk_resolve_attachments_;
  std::vector<uint32_t> preserve_attachments_;
};

/// A single renderpass object encapsulates multiple passes or rendering phases
/// over a single set of output images. Renderpass objects can contain multiple
/// subpasses.
/// Vulkan can figure out which subpasses are dependent on one another. However,
/// there are cases in which dependencies cannot be figured out automatically
/// (for example, when a subpass writes directly to a resource and a subsequent
/// subpass reads data back). Then dependencies must be explicitly defined.
class RenderPass {
public:
  /// Holds a renderpass reference containing the renderpass handle and the
  /// respective logical device handle
  class Ref {
    friend class RenderPass;
  public:
    Ref() = default;
    ~Ref();
    [[nodiscard]] VkRenderPass handle() const;
    [[nodiscard]] bool good() const;
  private:
    /// \param vk_image_handle
    explicit Ref(VkRenderPass vk_renderpass_handle);
    VkRenderPass vk_renderpass_{VK_NULL_HANDLE};
  };
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  RenderPass();
  explicit RenderPass(LogicalDevice::Ref logical_device);
  RenderPass(const RenderPass &other) = delete;
  RenderPass(RenderPass &&other) noexcept;
  ~RenderPass();
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  RenderPass &operator=(const RenderPass &other) = delete;
  RenderPass &operator=(RenderPass &&other) noexcept;
  // ***********************************************************************
  //                           CREATION
  // ***********************************************************************
  /// Initializes renderpass object
  bool init(const LogicalDevice::Ref& logical_device);
  void destroy();
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  [[nodiscard]] Ref ref() const;
  /// \return
  [[nodiscard]] bool good() const;
  ///\brief
  /// The load/store ops parameters specify what to do with the attachment at
  /// the beginning and end of the render pass
  ///\param format **[in]** format of the attachment (must match the format of
  /// the image attached)
  ///\param samples **[in]** indicates the number of samples used for
  /// multisampling (ex: VK_SAMPLE_COUNT_1_BIT)
  ///\param load_op **[in]**
  ///\param store_op **[in]**
  ///\param stencil_load_op **[in]**
  ///\param stencil_store_op **[in]**
  ///\param initial_layout **[in]** what layout to expect the image to be in
  /// when the renderpass begins
  ///\param final_layout **[in]** what layout to leave when the renderpass ends
  ///\return uint32_t attachment index
  uint32_t addAttachment(VkFormat format, VkSampleCountFlagBits samples,
                         VkAttachmentLoadOp load_op,
                         VkAttachmentStoreOp store_op,
                         VkAttachmentLoadOp stencil_load_op,
                         VkAttachmentStoreOp stencil_store_op,
                         VkImageLayout initial_layout,
                         VkImageLayout final_layout);
  ///\brief Defines a dependency between subpasses.
  ///\param src_subpass **[in]** source index in the subpass array
  ///\param dst_subpass **[in]** destination index in the subpass array
  ///\param src_stage_mask **[in]** specifies which pipeline stages produced
  /// data on the source subpass
  ///\param dst_stage_mask **[in]** specifies which pipeline stages will consume
  /// the data in the destination subpass
  ///\param src_access **[in]** specifies how the source subpass access the data
  ///\param dst_access **[in]** specifies how the destination subpass access the
  /// data
  void addSubpassDependency(uint32_t src_subpass, uint32_t dst_subpass,
                            VkPipelineStageFlags src_stage_mask,
                            VkPipelineStageFlags dst_stage_mask,
                            VkAccessFlags src_access, VkAccessFlags dst_access);
  // ***********************************************************************
  //                           FIELDS
  // ***********************************************************************
  ///\param id **[out | optional]** receives the subpass index
  ///\return SubpassDescription&
  SubpassDescription &newSubpassDescription(uint32_t *id = nullptr);
  [[nodiscard]] VkRenderPass handle() const;

private:
  LogicalDevice::Ref logical_device_;
  VkRenderPass vk_renderpass_ = VK_NULL_HANDLE;
  std::vector<VkAttachmentDescription> vk_attachments_;
  std::vector<VkSubpassDependency> vk_subpass_dependencies_;
  std::vector<SubpassDescription> subpass_descriptions_;
};

/// A framebuffer is an object that represents the set of images that graphics
/// pipelines render into. It is created by using a reference to a renderpass
/// and can be used with any renderpass that has a similar arrangement of
/// attachments.
class Framebuffer {
public:
  // ***********************************************************************
  //                           CONSTRUCTORS
  // ***********************************************************************
  Framebuffer();
  ///\param logical_device **[in]**
  ///\param renderpass **[in]** A compatible renderpass
  ///\param resolution **[in]**
  ///\param layers **[in]**
  Framebuffer(const LogicalDevice::Ref &logical_device, const RenderPass::Ref &renderpass,
              VkExtent2D resolution, u32 layers);
  Framebuffer(const Framebuffer &other) = delete;
  Framebuffer(Framebuffer &&other) noexcept;
  ~Framebuffer();
  // ***********************************************************************
  //                           OPERATORS
  // ***********************************************************************
  Framebuffer &operator=(const Framebuffer &other) = delete;
  Framebuffer &operator=(Framebuffer &&other) noexcept;
  // ***********************************************************************
  //                           CREATION
  // ***********************************************************************
  bool init();
  void destroy();
  [[nodiscard]] bool good() const;
  // ***********************************************************************
  //                           METHODS
  // ***********************************************************************
  ///\brief Bounds an image into the framebuffer
  /// The passes comprising the renderpass make references to the image
  /// attachments, and those refrences are specified as indices of the array
  /// constructed from theses additions. In order to make the framebuffer
  /// compatible to a renderpass, you are allowed to add image views with
  /// a null handle (VkNullHandle).
  ///\param image_view **[in]**
  void addAttachment(const Image::View &image_view);
  // ***********************************************************************
  //                           FIELDS
  // ***********************************************************************
  [[nodiscard]] VkFramebuffer handle() const;
  /// \return image resolution
  [[nodiscard]] VkExtent2D size() const;
  [[nodiscard]] u32 layers() const;

private:
  LogicalDevice::Ref logical_device_;
  VkExtent2D resolution_{};
  u32 layers_{};
  RenderPass::Ref renderpass_;
  VkFramebuffer vk_framebuffer_ = VK_NULL_HANDLE;
  std::vector<VkImageView> attachments_;
};

} // namespace circe

#endif // CIRCE_VULKAN_RENDERPASS_H