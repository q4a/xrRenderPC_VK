#include "device/device.h"
#include "backend/backend.h"

#include "frontend/render.h"


/**
 *
 */
void FrontEnd::SetupGPU
        ( bool force_sw
        , bool force_nonpure
        , bool force_ref
        )
{
    /*
     * Nothing here.
     * We don't support concept of reference devices and
     * expect all TnL processing on HW side.
     */
}


/**
 *
 */
bool
FrontEnd::GetForceGPU_REF()
{
    /*
     * Nothing here.
     * We don't support concept of reference devices
     */
    return false;
}


/**
 *
 */
LPCSTR
FrontEnd::getShaderPath()
{
    return "r3" DELIMITER ""; // we're using R3 shaders
}

/**
 *
 */
void
FrontEnd::CreateCommandBuffers()
{
    const auto cmd_pool_create_info = vk::CommandPoolCreateInfo()
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(hw.graphicsQfamilyIdx);

    cmd_pool_ = hw.device->createCommandPoolUnique(cmd_pool_create_info);

    const auto cmd_buffers_count = hw.baseRt.size();
    const auto cmd_buffer_alloc_info = vk::CommandBufferAllocateInfo()
        .setCommandPool(cmd_pool_.get())
        .setCommandBufferCount(cmd_buffers_count)
        .setLevel(vk::CommandBufferLevel::ePrimary);
    
    draw_cmd_buffers_ =
        hw.device->allocateCommandBuffersUnique(cmd_buffer_alloc_info);
}


/**
 *
 */
void
FrontEnd::DestroyCommandBuffers()
{
    for (auto& buffer : draw_cmd_buffers_)
    {
        buffer.reset();
    }

    cmd_pool_.reset();
}


/**
 *
 */
void
FrontEnd::CreateRenderPass()
{
    // A dummy render pass with one subpass only
    // (no PP, deferred and other fancy stuff yet)

    const auto color_attachment_description = vk::AttachmentDescription()
        .setFormat(hw.caps.colorFormat)
        .setLoadOp( psDeviceFlags.test(rsClearBB)
                    ? vk::AttachmentLoadOp::eClear
                    : vk::AttachmentLoadOp::eDontCare
                  )
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    const auto color_attachment_reference = vk::AttachmentReference()
        .setAttachment(0)
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    const auto subpass = vk::SubpassDescription()
        .setColorAttachmentCount(1)
        .setPColorAttachments(&color_attachment_reference)
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    const auto renderpass_create_info = vk::RenderPassCreateInfo()
        .setAttachmentCount(1)
        .setPAttachments(&color_attachment_description)
        .setSubpassCount(1)
        .setPSubpasses(&subpass);

    renderpass_ = hw.device->createRenderPassUnique(renderpass_create_info);

    // Attach SC images to render pass
    for (auto& resource : hw.baseRt)
    {
        const auto framebuffer_create_info = vk::FramebufferCreateInfo()
            .setRenderPass(renderpass_.get())
            .setAttachmentCount(1)
            .setPAttachments(&resource.imageView)
            .setWidth(hw.draw_rect.width)
            .setHeight(hw.draw_rect.height)
            .setLayers(1); // not stereo

        resource.frameBuffer =
            hw.device->createFramebuffer(framebuffer_create_info);
    }
}


/**
 *
 */
void
FrontEnd::DestroyRenderPass()
{
    for (auto& resource : hw.baseRt)
    {
        //resource.frameBuffer;
    }
    renderpass_.reset();
}


/**
 *
 */
void
FrontEnd::Create
        ( SDL_Window *hwnd
        , u32& width
        , u32& height
        , float& width_2
        , float& height_2
        )
{
    hw.CreateDevice(hwnd);

    CreateCommandBuffers();

    CreateRenderPass();

    // create semaphores
    frame_semaphores_.resize(hw.baseRt.size());
    for (auto& semaphore : frame_semaphores_)
    {
        semaphore = hw.device->createSemaphoreUnique({});
    }

    render_semaphores_.resize(hw.baseRt.size());
    for (auto& semaphore : render_semaphores_)
    {
        semaphore = hw.device->createSemaphoreUnique({});
    }
}


/**
 *
 */
void
FrontEnd::DestroyHW()
{
    DestroyRenderPass();

    DestroyCommandBuffers();

    hw.DestroyDevice();
}


/**
 *
 */
void
FrontEnd::OnDeviceCreate
        ( LPCSTR shader
        )
{
    backend.OnDeviceCreate();

    resources.OnDeviceCreate(shader);
}


/**
 *
 */
DeviceState
FrontEnd::GetDeviceState()
{
    DeviceState result = DeviceState::Normal;

    switch (swapchain_state_)
    {
    case vk::Result::eSuccess:
        // Nothing to do, continue;
        break;

    case vk::Result::eNotReady: // not ready, just wait
    case vk::Result::eTimeout: // a surface wasn't acquired, wait and try again
    case vk::Result::eErrorDeviceLost:
    case vk::Result::eErrorSurfaceLostKHR:
        // TODO: surface lost. What should we do?
        result = DeviceState::Lost;
        break;

    case vk::Result::eErrorOutOfDateKHR:
    case vk::Result::eSuboptimalKHR:
        // Surface params have been changed, need to re-create the swapchain
        result = DeviceState::NeedReset;
        break;

    default:
        // All other stuff like out-of-memory
        R_ASSERT2(false, "Something went wrong with presentation device..");
    };

    return result;
}


/**
 *
 */
void
FrontEnd::OnDeviceDestroy
        ( bool keep_textures
        )
{
    backend.OnDeviceDestroy();
}


/**
 *
 */
void
FrontEnd::Begin()
{
    swapchain_state_ = hw.device->acquireNextImageKHR(
          hw.swapchain
        , std::numeric_limits<std::uint64_t>::max()
        , frame_semaphores_[current_image_].get()
        , nullptr
        , &current_image_
    );

    auto& cmd_buffer = draw_cmd_buffers_[current_image_];
    auto& swapchain_resource = hw.baseRt[current_image_];

    const auto begin_info = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
    
    cmd_buffer->begin(begin_info);

    // Begin render pass
    const auto renderpass_begin_info = vk::RenderPassBeginInfo()
        .setRenderPass(renderpass_.get())
        .setFramebuffer(swapchain_resource.frameBuffer)
        .setRenderArea(vk::Rect2D( vk::Offset2D(0, 0)
                                 , vk::Extent2D( hw.draw_rect.width
                                               , hw.draw_rect.height
        )));
//        .setClearValueCount(1)
//        .setPClearValues(&clearValue);

    cmd_buffer->beginRenderPass( renderpass_begin_info
                               , vk::SubpassContents::eInline
    );

    backend.OnFrameBegin();
}


/**
 *
 */
void
FrontEnd::End()
{
    backend.OnFrameEnd();

    auto& cmd_buffer = draw_cmd_buffers_[current_image_];

    cmd_buffer->endRenderPass();
    cmd_buffer->end();

    const vk::PipelineStageFlags wait_mask =
        vk::PipelineStageFlagBits::eTransfer;
    const auto submit_info = vk::SubmitInfo()
        .setWaitSemaphoreCount(1)
        .setPWaitDstStageMask(&wait_mask)
        .setPWaitSemaphores(&frame_semaphores_[current_image_].get())
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmd_buffer.get())
        .setSignalSemaphoreCount(1)
        .setPSignalSemaphores(&render_semaphores_[current_image_].get());

    hw.submission_q.submit(submit_info, {});

    // Presentation
    const auto present_info = vk::PresentInfoKHR()
        .setWaitSemaphoreCount(1)
        .setPWaitSemaphores(&render_semaphores_[current_image_].get())
        .setSwapchainCount(1)
        .setPSwapchains(&hw.swapchain)
        .setPImageIndices(&current_image_);
    
    hw.submission_q.presentKHR(present_info);
    current_image_ = (current_image_ + 1) % hw.baseRt.size();
}
