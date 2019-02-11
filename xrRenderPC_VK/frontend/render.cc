#include "frontend/render.h"

#include "device/device.h"


FrontEnd frontend;


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

}


/**
 *
 */
void
FrontEnd::DestroyRenderPass()
{

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

    // create frame semaphores
    const auto semaphore_create_info = vk::SemaphoreCreateInfo();

    frame_semaphores_.resize(hw.baseRt.size());
    for (auto& semaphore : frame_semaphores_)
    {
        semaphore = hw.device->createSemaphoreUnique(semaphore_create_info);
    }
}


/**
 *
 */
void
FrontEnd::Destroy()
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
    // backend init
    // RM init
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
    case vk::Result::eTimeout: // a surface didn't acquired, wait and try again
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
    // backend destruction
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
}


/**
 *
 */
void
FrontEnd::End()
{
    const auto present_info = vk::PresentInfoKHR()
        .setSwapchainCount(1)
        .setPSwapchains(&hw.swapchain)
        .setPImageIndices(&current_image_);
    
    hw.submission_q.presentKHR(present_info);
    current_image_ = (current_image_ + 1) % hw.baseRt.size();
}
