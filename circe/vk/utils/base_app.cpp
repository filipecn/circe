//
// Created by filipecn on 09/06/2021.
//


/// Copyright (c) 2021, FilipeCN.
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
///\file base_app.cpp.c
///\author FilipeCN (filipedecn@gmail.com)
///\date 2021-06-09
///
///\brief

#include <circe/vk/utils/base_app.h>
#include <circe/vk/utils/vk_debug.h>

namespace circe::vk {

BaseApp::BaseApp(const hermes::size2 &window_size, const std::string &title) :
    window_{GraphicsDisplay(window_size, title)} {
  window_.resize_callback = [&](int new_w, int new_h) {
    render_engine_.resize({static_cast<unsigned int>(new_w), static_cast<unsigned int>(new_h)});
  };
}

BaseApp::~BaseApp() = default;

void BaseApp::initInstance() {
  HERMES_ASSERT(instance_.init("hello_vulkan_app",
                              circe::vk::GraphicsDisplay::requiredVkExtensions(),
                              {"VK_LAYER_KHRONOS_validation"}))
}

void BaseApp::initPresentationSurface() {
  HERMES_ASSERT((presentation_surface_ = window_.createWindowSurface(instance_)).good())
}

void BaseApp::initPhysicalDeviceAndFamilyQueues() {
  HERMES_ASSERT((physical_device_ =
                    instance_.pickPhysicalDevice(queue_families_, presentation_surface_.handle())).good())

  // compute depth format
  physical_device_.findSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
       VK_FORMAT_D24_UNORM_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
      depth_format_);

  // setup anti-aliasing
  msaa_samples_ = physical_device_.maxUsableSampleCount();
}

void BaseApp::initLogicalDevice() {
  VkPhysicalDeviceFeatures features = {};
  features.samplerAnisotropy = VK_TRUE;
  HERMES_ASSERT(device_.init(&physical_device_, {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
                            features, queue_families_, {"VK_LAYER_KHRONOS_validation"}))
}

void BaseApp::initRenderEngine() {
  HERMES_ASSERT(render_engine_.setupDevice(device_,
                                          queue_families_.family("graphics").family_index.value()))
  HERMES_ASSERT(render_engine_.setPresentationSurface(presentation_surface_))
  // register callbacks
  render_engine_.create_swap_chain_callback = [&]() { createSwapChain(); };
  render_engine_.destroy_swap_chain_callback = [&]() { destroySwapChain(); };
  render_engine_.prepare_frame_callback = [&](uint32_t index) {
    prepareFrameImage(index);
  };
  render_engine_.record_command_buffer_callback = [&](CommandBuffer &cb, u32 i) {
    recordCommandBuffer(cb, i);
  };
  render_engine_.resize_callback = [&](const hermes::size2 &new_window_size) {
    resize(new_window_size);
  };
  // init render engine
  render_engine_.init();
}

void BaseApp::initRenderpass() {
  auto &subpass_desc = renderpass_.newSubpassDescription();

  { // COLOR ATTACHMENT
    renderpass_.addAttachment(
        render_engine_.swapchain().surfaceFormat().format,
        msaa_samples_, VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED,
        // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    subpass_desc.addColorAttachmentRef(
        0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    renderpass_.addSubpassDependency(
        VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
        // VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
  }
  { // DEPTH ATTACHMENT
    renderpass_.addAttachment(
        depth_format_, msaa_samples_,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    subpass_desc.setDepthStencilAttachmentRef(
        1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  }
  // since we are using multi-sampling, the first color attachment cannot be
  // presented directly, first we need to resolve it into a proper image
  { // COLOR RESOLVE ATTACHMENT RESOLVE
    renderpass_.addAttachment(
        render_engine_.swapchain().surfaceFormat().format,
        VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    subpass_desc.addResolveAttachmentRef(
        2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  }
  HERMES_ASSERT(renderpass_.init(device_.ref()))
}

void BaseApp::init() {
  initInstance();
  initPresentationSurface();
  initPhysicalDeviceAndFamilyQueues();
  initLogicalDevice();
  initRenderEngine();
  initRenderpass();
}

void BaseApp::createSwapChain() {
  const auto &swapchain = render_engine_.swapchain();

  // COLOR RESOURCES (anti-aliasing)
  color_image_.init(device_.ref(), VK_IMAGE_TYPE_2D, swapchain.imageFormat(),
                    {swapchain.imageSize().width, swapchain.imageSize().height, 1}, 1, 1,
                    msaa_samples_, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                    false);
  color_image_memory_ = DeviceMemory(color_image_, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  color_image_memory_.bind(color_image_);
  color_image_view_ = color_image_.view(VK_IMAGE_VIEW_TYPE_2D, swapchain.imageFormat(),
                                        VK_IMAGE_ASPECT_COLOR_BIT);
  // DEPTH BUFFER
  depth_image_.init(
      device_.ref(), VK_IMAGE_TYPE_2D, depth_format_,
      {swapchain.imageSize().width, swapchain.imageSize().height, 1}, 1, 1,
      msaa_samples_, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, false);
  depth_image_memory_ = DeviceMemory(depth_image_, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  depth_image_memory_.bind(depth_image_);
  depth_image_view_ = depth_image_.view(VK_IMAGE_VIEW_TYPE_2D, depth_format_, VK_IMAGE_ASPECT_DEPTH_BIT);

  // setup framebuffers
  auto &swapchain_image_views = render_engine_.swapchainImageViews();
  for (auto &image_view : swapchain_image_views) {
    framebuffers_.emplace_back(device_.ref(), renderpass_.ref(),
                               swapchain.imageSize(), 1);
    // this order must be the same as the renderpass attachments
    framebuffers_.back().addAttachment(color_image_view_);
    framebuffers_.back().addAttachment(depth_image_view_);
    framebuffers_.back().addAttachment(image_view);
    framebuffers_.back().init();
  }
}

void BaseApp::destroySwapChain() {
  // destroy presentation objects
  color_image_view_.destroy();
  color_image_.destroy();
  color_image_memory_.destroy();
  depth_image_view_.destroy();
  depth_image_.destroy();
  depth_image_memory_.destroy();
  framebuffers_.clear();
}

int BaseApp::run() {
  init();
  prepare();
  render_engine_.recreateSwapChain();
  auto draw_callback = [&]() {
    nextFrame();
  };
  window_.open(draw_callback);
  return 0;
}

void BaseApp::render() {
  render_engine_.draw(queue_families_.family("graphics").vk_queues[0],
                      queue_families_.family("presentation").vk_queues[0]);
}

void BaseApp::nextFrame() {
  // start frame time
  auto t_start = std::chrono::high_resolution_clock::now();
  // update scene
  render();
  frame_counter_++;
  auto t_end = std::chrono::high_resolution_clock::now();
  auto t_diff = std::chrono::duration<double, std::milli>(t_end - t_start).count();
  frame_timer_ = (float) t_diff / 1000.0f;
  // Convert to clamped timer value
  float fps_timer = (float) (std::chrono::duration<double, std::milli>(t_end - last_timestamp_).count());
  if (fps_timer > 1000.0f) {
    last_FPS_ = static_cast<uint32_t>((float) frame_counter_ * (1000.0f / fps_timer));
    frame_counter_ = 0;
    last_timestamp_ = t_end;
  }
}

}
