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
FrontEnd::Create
        ( SDL_Window *hwnd
        , u32& width
        , u32& height
        , float& width_2
        , float& height_2
        )
{
    hw.CreateDevice(hwnd);
}


/**
 *
 */
void
FrontEnd::DestroyHW()
{
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
    const auto frame_semaphore =
        backend.frame_semaphores[current_image_].get();

    swapchain_state_ = hw.device->acquireNextImageKHR(
          hw.swapchain
        , std::numeric_limits<std::uint64_t>::max()
        , frame_semaphore
        , nullptr
        , &current_image_
    );

    backend.OnFrameBegin(current_image_);
}


/**
 *
 */
void
FrontEnd::End()
{
    backend.OnFrameEnd(current_image_);

    // Presentation
    const auto render_semaphore =
        backend.render_semaphores[current_image_].get();

    const auto present_info = vk::PresentInfoKHR()
        .setWaitSemaphoreCount(1)
        .setPWaitSemaphores(&render_semaphore)
        .setSwapchainCount(1)
        .setPSwapchains(&hw.swapchain)
        .setPImageIndices(&current_image_);
    
    hw.submission_q.presentKHR(present_info);
    current_image_ = (current_image_ + 1) % hw.baseRt.size();

    #ifdef DEBUG
    hw.device->waitIdle();
    #endif
}
