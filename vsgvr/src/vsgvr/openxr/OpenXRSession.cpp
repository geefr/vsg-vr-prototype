#include <vsgvr/openxr/OpenXRSession.h>

#include <iostream>

#include <vsg/core/Exception.h>
#include <vsg/commands/PipelineBarrier.h>
#include <vsg/vk/SubmitCommands.h>

#include "OpenXRMacros.cpp"

using namespace vsg;

namespace vsgvr {

  OpenXRSession::OpenXRSession(XrInstance instance, XrSystemId system, vsg::ref_ptr<OpenXRGraphicsBindingVulkan2> graphicsBinding,
                               VkFormat swapchainFormat, std::vector<XrViewConfigurationView> viewConfigs)
    : _graphicsBinding(graphicsBinding)
  {
    createSession(instance, system);
    createSwapchains(swapchainFormat, viewConfigs);
  }

  OpenXRSession::~OpenXRSession()
  {
    destroySwapchains();
    destroySession();
  }

  // TODO: A regular session update is needed - should handle the session lifecycle and such
  // void update(XrInstance instance, XrSystemId system);

  void OpenXRSession::createSession(XrInstance instance, XrSystemId system)
  {
    auto info = XrSessionCreateInfo();
    info.type = XR_TYPE_SESSION_CREATE_INFO;
    info.next = &_graphicsBinding->getBinding();
    info.systemId = system;

    xr_check(xrCreateSession(instance, &info, &_session), "Failed to create OpenXR session");
    _sessionState = XR_SESSION_STATE_IDLE;

    auto spaceCreateInfo = XrReferenceSpaceCreateInfo();
    spaceCreateInfo.type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
    spaceCreateInfo.next = nullptr;
    spaceCreateInfo.poseInReferenceSpace.orientation = XrQuaternionf{ 0.0f, 0.0f, 0.0f, 1.0f };
    spaceCreateInfo.poseInReferenceSpace.position = XrVector3f{ 0.0f, 0.0f, 0.0f };
    spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;

    xr_check(xrCreateReferenceSpace(_session, &spaceCreateInfo, &_space), "Failed to create Session reference space");
  }

  void OpenXRSession::createSwapchains(VkFormat swapchainFormat, std::vector<XrViewConfigurationView> viewConfigs)
  {
    if( !_viewData.empty() ) throw Exception({ "Swapchain already initialised" });
    if (!_session) throw Exception({ "Unable to create swapchain without session" });

    for( auto& viewConfig : viewConfigs )
    {
      PerViewData v;
      v.swapchain = OpenXRSwapchain::create(_session, swapchainFormat, viewConfig, _graphicsBinding);

      auto extent = v.swapchain->getExtent();
      VkSampleCountFlagBits framebufferSamples;
      switch (viewConfig.recommendedSwapchainSampleCount)
      {
         case 1:
           framebufferSamples = VK_SAMPLE_COUNT_1_BIT;
           break;
         case 2:
           framebufferSamples = VK_SAMPLE_COUNT_2_BIT;
           break;
         case 4:
           framebufferSamples = VK_SAMPLE_COUNT_4_BIT;
           break;
         case 8:
           framebufferSamples = VK_SAMPLE_COUNT_8_BIT;
           break;
         case 16:
           framebufferSamples = VK_SAMPLE_COUNT_16_BIT;
           break;
         case 32:
           framebufferSamples = VK_SAMPLE_COUNT_32_BIT;
           break;
         case 64:
           framebufferSamples = VK_SAMPLE_COUNT_64_BIT;
           break;
      }

      bool multisampling = framebufferSamples != VK_SAMPLE_COUNT_1_BIT;
      if (multisampling)
      {
        v.multisampleImage = Image::create();
        v.multisampleImage->imageType = VK_IMAGE_TYPE_2D;
        v.multisampleImage->format = swapchainFormat;
        v.multisampleImage->extent.width = extent.width;
        v.multisampleImage->extent.height = extent.height;
        v.multisampleImage->extent.depth = 1;
        v.multisampleImage->mipLevels = 1;
        v.multisampleImage->arrayLayers = 1;
        v.multisampleImage->samples = framebufferSamples;
        v.multisampleImage->tiling = VK_IMAGE_TILING_OPTIMAL;
        v.multisampleImage->usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        v.multisampleImage->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        v.multisampleImage->flags = 0;
        v.multisampleImage->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        v.multisampleImage->compile(_graphicsBinding->getVkDevice());
        v.multisampleImage->allocateAndBindMemory(_graphicsBinding->getVkDevice());

        v.multisampleImageView = ImageView::create(v.multisampleImage, VK_IMAGE_ASPECT_COLOR_BIT);
        v.multisampleImageView->compile(_graphicsBinding->getVkDevice());
      }

      bool requiresDepthRead = false; // (_traits->depthImageUsage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) != 0;

      // Window::_initRenderPass
      // TODO: Test single & multisampled rendering
      {
        if (framebufferSamples == VK_SAMPLE_COUNT_1_BIT)
        {
          v.renderPass = vsg::createRenderPass(_graphicsBinding->getVkDevice(), swapchainFormat, VK_FORMAT_D32_SFLOAT, requiresDepthRead);
        }
        else
        {
          v.renderPass = vsg::createMultisampledRenderPass(_graphicsBinding->getVkDevice(), swapchainFormat, VK_FORMAT_D32_SFLOAT, framebufferSamples, requiresDepthRead);
        }
      }

      bool requiresDepthResolve = (multisampling && requiresDepthRead);

      // create depth buffer
      v.depthImage = Image::create();
      v.depthImage->imageType = VK_IMAGE_TYPE_2D;
      v.depthImage->extent.width = extent.width;
      v.depthImage->extent.height = extent.height;
      v.depthImage->extent.depth = 1;
      v.depthImage->mipLevels = 1;
      v.depthImage->arrayLayers = 1;
      v.depthImage->format = VK_FORMAT_D32_SFLOAT; // _depthFormat;
      v.depthImage->tiling = VK_IMAGE_TILING_OPTIMAL;
      v.depthImage->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      v.depthImage->samples = framebufferSamples;
      v.depthImage->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      v.depthImage->usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; // _traits->depthImageUsage;

      v.depthImage->compile(_graphicsBinding->getVkDevice());
      v.depthImage->allocateAndBindMemory(_graphicsBinding->getVkDevice());

      v.depthImageView = ImageView::create(v.depthImage);
      v.depthImageView->compile(_graphicsBinding->getVkDevice());

      if (requiresDepthResolve)
      {
        v.multisampleDepthImage = v.depthImage;
        v.multisampleDepthImageView = v.depthImageView;

        // create depth buffer
        v.depthImage = Image::create();
        v.depthImage->imageType = VK_IMAGE_TYPE_2D;
        v.depthImage->extent.width = extent.width;
        v.depthImage->extent.height = extent.height;
        v.depthImage->extent.depth = 1;
        v.depthImage->mipLevels = 1;
        v.depthImage->arrayLayers = 1;
        v.depthImage->format = VK_FORMAT_D32_SFLOAT; // _depthFormat;
        v.depthImage->tiling = VK_IMAGE_TILING_OPTIMAL;
        v.depthImage->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        v.depthImage->usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; // _traits->depthImageUsage;
        v.depthImage->samples = VK_SAMPLE_COUNT_1_BIT;
        v.depthImage->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        v.depthImage->compile(_graphicsBinding->getVkDevice());
        v.depthImage->allocateAndBindMemory(_graphicsBinding->getVkDevice());

        v.depthImageView = ImageView::create(v.depthImage);
        v.depthImageView->compile(_graphicsBinding->getVkDevice());
      }

      int graphicsFamily = _graphicsBinding->getVkPhysicalDevice()->getQueueFamily(VK_QUEUE_GRAPHICS_BIT);

      // set up framebuffer and associated resources
      auto& imageViews = v.swapchain->getImageViews();

      size_t initial_indexValue = imageViews.size();
      for (size_t i = 0; i < imageViews.size(); ++i)
      {
        vsg::ImageViews attachments;
        if (v.multisampleImageView)
        {
          attachments.push_back(v.multisampleImageView);
        }
        attachments.push_back(imageViews[i]);

        if (v.multisampleDepthImageView)
        {
          attachments.push_back(v.multisampleDepthImageView);
        }
        attachments.push_back(v.depthImageView);

        ref_ptr<Framebuffer> fb = Framebuffer::create(v.renderPass, attachments, extent.width, extent.height, 1);

        v.frames.push_back({ imageViews[i], fb });
      }

      {
        // ensure image attachments are setup on GPU.
        ref_ptr<CommandPool> commandPool = CommandPool::create(_graphicsBinding->getVkDevice(), graphicsFamily);
        submitCommandsToQueue(_graphicsBinding->getVkDevice(), commandPool, _graphicsBinding->getVkDevice()->getQueue(graphicsFamily), [&](CommandBuffer& commandBuffer) {
          auto depthImageBarrier = ImageMemoryBarrier::create(
            0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
            v.depthImage,
            v.depthImageView->subresourceRange);

          auto pipelineBarrier = PipelineBarrier::create(
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            0, depthImageBarrier);
          pipelineBarrier->record(commandBuffer);

          if (multisampling)
          {
            auto msImageBarrier = ImageMemoryBarrier::create(
              0, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
              VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
              VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
              v.multisampleImage,
              VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
            auto msPipelineBarrier = PipelineBarrier::create(
              VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
              0, msImageBarrier);
            msPipelineBarrier->record(commandBuffer);
          }
          });
      }

      _viewData.push_back(std::move(v));
    }
  }

  void OpenXRSession::destroySwapchains()
  {
    _viewData.clear();
  }

  void OpenXRSession::destroySession()
  {
    xr_check(xrDestroySpace(_space));
    _space = nullptr;
    xr_check(xrDestroySession(_session));
    _session = nullptr;
  }

  void OpenXRSession::beginSession(XrViewConfigurationType viewConfigurationType)
  {
    if (_sessionRunning) return;
    auto info = XrSessionBeginInfo();
    info.type = XR_TYPE_SESSION_BEGIN_INFO;
    info.next = nullptr;
    info.primaryViewConfigurationType = viewConfigurationType;
    xr_check(xrBeginSession(_session, &info), "Failed to begin session");
    _sessionRunning = true;
  }

  void OpenXRSession::endSession()
  {
    if (!_sessionRunning) return;
    xr_check(xrEndSession(_session), "Failed to end session");
    _sessionRunning = false;
  }

  void OpenXRSession::onEventStateChanged(const XrEventDataSessionStateChanged& event)
  {
    // TODO: Update _sessionState, handle the state change properly (Will be polled for in update?)
    //       Should the session trasition logic be here, or out in Instance? (Should Instance be renamed? It's more like a Viewer)
    _sessionState = event.state;
    std::cerr << "Session state changed: " << to_string(_sessionState) << std::endl;
  }

}