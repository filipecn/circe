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
///\file vk_renderpass.cpp
///\author FilipeCN (filipedecn@gmail.com)
///\date 2019-11-06
///
///\brief

#include <circe/vk/pipeline/renderpass.h>
#include <circe/vk/utils/vk_debug.h>

namespace circe::vk {

SubpassDescription::SubpassDescription()
    : depth_stencil_attachment_set_(false) {}

uint32_t SubpassDescription::addInputAttachmentRef(uint32_t attachment,
                                                   VkImageLayout layout) {
  VkAttachmentReference ar = {attachment, layout};
  vk_input_attachments_.emplace_back(ar);
  return vk_input_attachments_.size() - 1;
}

uint32_t SubpassDescription::addColorAttachmentRef(uint32_t attachment,
                                                   VkImageLayout layout) {
  VkAttachmentReference ar = {attachment, layout};
  vk_color_attachments_.emplace_back(ar);
  return vk_color_attachments_.size() - 1;
}

uint32_t
SubpassDescription::addResolveAttachmentRef(uint32_t resolve_attachment,
                                            VkImageLayout resolve_layout) {
  VkAttachmentReference rar = {resolve_attachment, resolve_layout};
  vk_resolve_attachments_.emplace_back(rar);
  return vk_resolve_attachments_.size() - 1;
}

void SubpassDescription::setDepthStencilAttachmentRef(uint32_t attachment,
                                                      VkImageLayout layout) {
  vk_depth_stencil_attachment_.attachment = attachment;
  vk_depth_stencil_attachment_.layout = layout;
  depth_stencil_attachment_set_ = true;
}

void SubpassDescription::preserveAttachment(uint32_t attachment) {
  preserve_attachments_.emplace_back(attachment);
}

const VkAttachmentReference &
SubpassDescription::depthStencilAttachmentRef() const {
  return vk_depth_stencil_attachment_;
}

const std::vector<VkAttachmentReference> &
SubpassDescription::inputAttachmentRefs() const {
  return vk_input_attachments_;
}

const std::vector<VkAttachmentReference> &
SubpassDescription::colorAttachmentRefs() const {
  return vk_color_attachments_;
}

const std::vector<VkAttachmentReference> &
SubpassDescription::resolveAttachmentRefs() const {
  return vk_resolve_attachments_;
}

const std::vector<uint32_t> &
SubpassDescription::preserveAttachmentRefs() const {
  return preserve_attachments_;
}

bool SubpassDescription::hasDepthStencilAttachmentRef() const {
  return depth_stencil_attachment_set_;
}

RenderPass::RenderPass() = default;

RenderPass::RenderPass(LogicalDevice::Ref logical_device)
    : logical_device_(logical_device) {}

RenderPass::RenderPass(RenderPass &&other) noexcept {
  logical_device_ = other.logical_device_;
  vk_renderpass_ = other.vk_renderpass_;
  other.vk_renderpass_ = VK_NULL_HANDLE;
  vk_attachments_ = std::move(other.vk_attachments_);
  vk_subpass_dependencies_ = std::move(other.vk_subpass_dependencies_);
  subpass_descriptions_ = std::move(other.subpass_descriptions_);
}

RenderPass::~RenderPass() { destroy(); }

RenderPass &RenderPass::operator=(RenderPass &&other) noexcept {
  destroy();
  logical_device_ = other.logical_device_;
  vk_renderpass_ = other.vk_renderpass_;
  other.vk_renderpass_ = VK_NULL_HANDLE;
  vk_attachments_ = std::move(other.vk_attachments_);
  vk_subpass_dependencies_ = std::move(other.vk_subpass_dependencies_);
  subpass_descriptions_ = std::move(other.subpass_descriptions_);
  return *this;
}

bool RenderPass::init(const LogicalDevice::Ref& logical_device) {
  destroy();
  logical_device_ = logical_device;
  std::vector<VkSubpassDescription> subpass_descriptions;
  for (auto &sd : subpass_descriptions_) {
    VkSubpassDescription info = {
        0,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        static_cast<uint32_t>(sd.inputAttachmentRefs().size()),
        (!sd.inputAttachmentRefs().empty() ? sd.inputAttachmentRefs().data()
                                           : nullptr),
        static_cast<uint32_t>(sd.colorAttachmentRefs().size()),
        (!sd.colorAttachmentRefs().empty() ? sd.colorAttachmentRefs().data()
                                           : nullptr),
        (!sd.resolveAttachmentRefs().empty() ? sd.resolveAttachmentRefs().data()
                                             : nullptr),
        (sd.hasDepthStencilAttachmentRef() ? &sd.depthStencilAttachmentRef()
                                           : nullptr),
        static_cast<uint32_t>(sd.preserveAttachmentRefs().size()),
        (!sd.preserveAttachmentRefs().empty()
         ? sd.preserveAttachmentRefs().data()
         : nullptr)};
    subpass_descriptions.emplace_back(info);
  }
  VkRenderPassCreateInfo info = {
      VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      nullptr,
      0,
      static_cast<uint32_t>(vk_attachments_.size()),
      vk_attachments_.data(),
      static_cast<uint32_t>(subpass_descriptions.size()),
      subpass_descriptions.data(),
      static_cast<uint32_t>(vk_subpass_dependencies_.size()),
      vk_subpass_dependencies_.data()};
  PONOS_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  R_CHECK_VULKAN(vkCreateRenderPass(logical_device_.handle(), &info,
                                    nullptr, &vk_renderpass_), false)
  return true;
}

void RenderPass::destroy() {
  if (vk_renderpass_ != VK_NULL_HANDLE) {
    vkDestroyRenderPass(logical_device_.handle(), vk_renderpass_, nullptr);
    vk_renderpass_ = VK_NULL_HANDLE;
  }
}

RenderPass::Ref RenderPass::ref() const { return RenderPass::Ref(vk_renderpass_); }

bool RenderPass::good() const {
  return vk_renderpass_ != VK_NULL_HANDLE;
}

uint32_t RenderPass::addAttachment(
    VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp load_op,
    VkAttachmentStoreOp store_op, VkAttachmentLoadOp stencil_load_op,
    VkAttachmentStoreOp stencil_store_op, VkImageLayout initial_layout,
    VkImageLayout final_layout) {
  VkAttachmentDescription d = {
      0, format, samples, load_op,
      store_op, stencil_load_op, stencil_store_op, initial_layout,
      final_layout};
  vk_attachments_.emplace_back(d);
  return vk_attachments_.size() - 1;
}
void RenderPass::addSubpassDependency(uint32_t src_subpass,
                                      uint32_t dst_subpass,
                                      VkPipelineStageFlags src_stage_mask,
                                      VkPipelineStageFlags dst_stage_mask,
                                      VkAccessFlags src_access,
                                      VkAccessFlags dst_access) {
  VkSubpassDependency sd = {src_subpass,
                            dst_subpass,
                            src_stage_mask,
                            dst_stage_mask,
                            src_access,
                            dst_access,
                            0};
  vk_subpass_dependencies_.emplace_back(sd);
}
SubpassDescription &RenderPass::newSubpassDescription(uint32_t *id) {
  if (id)
    *id = subpass_descriptions_.size();
  subpass_descriptions_.emplace_back();
  return subpass_descriptions_[subpass_descriptions_.size() - 1];
}
VkRenderPass RenderPass::handle() const {
  return vk_renderpass_;
}

RenderPass::Ref::Ref(VkRenderPass vk_renderpass_handle) : vk_renderpass_{vk_renderpass_handle} {}

RenderPass::Ref::~Ref() = default;

VkRenderPass RenderPass::Ref::handle() const { return vk_renderpass_; }

bool RenderPass::Ref::good() const { return vk_renderpass_ != VK_NULL_HANDLE; }

Framebuffer::Framebuffer() = default;

Framebuffer::Framebuffer(const LogicalDevice::Ref &logical_device,
                         const RenderPass::Ref &renderpass,
                         VkExtent2D resolution, uint32_t layers)
    : logical_device_(logical_device), resolution_(resolution),
      layers_(layers), renderpass_(renderpass) {}

Framebuffer::Framebuffer(Framebuffer &&other) noexcept
    : Framebuffer(other.logical_device_, other.renderpass_, other.resolution_,
                  other.layers_) {
  attachments_ = std::move(other.attachments_);
  vk_framebuffer_ = other.vk_framebuffer_;
  other.vk_framebuffer_ = VK_NULL_HANDLE;
}

Framebuffer::~Framebuffer() {
  destroy();
}

Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept {
  return *this;
}

bool Framebuffer::init() {
  destroy();
  VkFramebufferCreateInfo info = {
      VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      nullptr,
      0,
      renderpass_.handle(),
      static_cast<uint32_t>(attachments_.size()),
      (!attachments_.empty() ? attachments_.data() : nullptr),
      resolution_.width,
      resolution_.height,
      layers_};
  PONOS_VALIDATE_EXP_WITH_WARNING(logical_device_.good(), "using bad device.")
  R_CHECK_VULKAN(vkCreateFramebuffer(logical_device_.handle(), &info,
                                     nullptr, &vk_framebuffer_), false)
  return true;
}

void Framebuffer::destroy() {
  if (vk_framebuffer_ != VK_NULL_HANDLE)
    vkDestroyFramebuffer(logical_device_.handle(), vk_framebuffer_, nullptr);
  vk_framebuffer_ = VK_NULL_HANDLE;
}

bool Framebuffer::good() const {
  return vk_framebuffer_ != VK_NULL_HANDLE;
}

void Framebuffer::addAttachment(const Image::View &image_view) {
  attachments_.emplace_back(image_view.handle());
}

VkExtent2D Framebuffer::size() const {
  return resolution_;
}

uint32_t Framebuffer::layers() const { return layers_; }

VkFramebuffer Framebuffer::handle() const {
  return vk_framebuffer_;
}

} // namespace circe