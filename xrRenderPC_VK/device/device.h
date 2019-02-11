#ifndef DEVICE_DEVICE_H_
#define DEVICE_DEVICE_H_

#include <limits>
#include <vector>

#include <SDL.h>
#include <SDL_syswm.h>
#include <vk_mem_alloc.h>

#include "common.h"
#include "caps.h"

constexpr std::uint32_t invalidQindex = std::numeric_limits<std::uint32_t>::max();

class Hw
    : public pureAppActivate
    , public pureAppDeactivate
{
    void CreateInstance();
    void SelectGpu();
    void CreateSurface(SDL_Window * const sdlWnd);
    void CreateLogicalDevice();
    void CreateSwapchain();
    void CreateCommandBuffers();

    vk::PresentModeKHR SelectPresentMode() const;
    vk::Format SelectColorFormat() const;
    vk::ColorSpaceKHR SelectColorSpace() const;
    vk::CompositeAlphaFlagBitsKHR SelectCompositeAlpha() const;

    void DestroyCommandBuffers();
    void DestroySwapchain();

public:
    struct SwapchainResource
    {
        vk::ImageView imageView;
        vk::Image image;
        vk::Framebuffer frameBuffer;
    };

    Hw() = default;
    ~Hw() = default;

    void CreateDevice(SDL_Window * const sdlWnd);
    void DestroyDevice();
    void ResetDevice();

    // Interface implementations
    void OnAppActivate() override;
    void OnAppDeactivate() override;

public:
    Caps caps; //< device capabilities 

    std::uint32_t presentQfamilyIdx  = invalidQindex;
    std::uint32_t graphicsQfamilyIdx = invalidQindex;

    vk::UniqueDevice device;
    vk::Queue submissionQ;

    // Swapchain
    vk::SwapchainKHR swapchain = nullptr;
    vk::Extent2D drawRect;  //< presentation surface dimensions
    std::vector<SwapchainResource> baseRt;

private:
    vk::UniqueInstance instance_;
    vk::PhysicalDevice gpu_; //< selected physical device
    vk::UniqueSurfaceKHR wsiSurface_;

    HWND hWnd_;

    vk::UniqueCommandPool cmd_pool_;
    std::vector<vk::UniqueCommandBuffer> ctrl_cmd_buf_; //< device control commands
};

extern Hw hw;

#endif // DEVICE_DEVICE_H_
