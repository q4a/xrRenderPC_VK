#ifndef DEVICE_DEVICE_H_
#define DEVICE_DEVICE_H_

#include <limits>
#include <vector>

#include <gli/gli.hpp>
#include <SDL.h>
#include <SDL_syswm.h>
#include <vk_mem_alloc.h>

#include "device/caps.h"
#include "device/memory.h"

constexpr std::uint32_t invalidQindex = std::numeric_limits<std::uint32_t>::max();

class Hw
    : public pureAppActivate
    , public pureAppDeactivate
{
    friend class StreamBuffer;
    friend class StreamImage;

    void CreateInstance();
    void SelectGpu();
    void CreateSurface(SDL_Window * const sdlWnd);
    void CreateLogicalDevice();
    void CreateSwapchain();
    void CreateCommandBuffers();
    void CreateMemoryAllocator();

    vk::PresentModeKHR SelectPresentMode() const;
    vk::Format SelectColorFormat() const;
    vk::Format SelectDepthStencilFormat() const;
    vk::ColorSpaceKHR SelectColorSpace() const;
    vk::CompositeAlphaFlagBitsKHR SelectCompositeAlpha() const;

    void DestroyMemoryAllocator();
    void DestroyCommandBuffers();
    void DestroySwapchain();

public:
    Hw() = default;
    ~Hw() = default;

    void CreateDevice(SDL_Window * const sdlWnd);
    void DestroyDevice();
    void ResetDevice();

    // Interface implementations
    void OnAppActivate() override;
    void OnAppDeactivate() override;

    /* Memory management
     */

    /*!
     * \brief   Allocates host memory buffer
     */
    BufferPtr CreateCpuBuffer( std::size_t size ///< buffer size
                             ) const;
    /*!
     * \brief   Allocates device memory buffer
     */
    BufferPtr CreateGpuBuffer( std::size_t size ///< buffer size
                             , BufferType  type ///< buffer type (Uniform/Index/Vertex)
                             ) const;
    ImagePtr CreateGpuImage( const vk::Extent3D &dimensions
                           , vk::Format          format
                           , std::uint32_t       layers_count
                           , std::uint32_t       levels_count
                           , ImageType           type = ImageType::Texture
                           ) const;
    /*!
     * \brief   Allocates device memory for image data
     */
    ImagePtr CreateGpuImage( const gli::texture &image_description ///< GLI `texture` descriptor
                           ) const;

    /*!
     * \brief   Transfers CPU buffer into device buffer
     */
    void Transfer( const BufferPtr &destination ///< [in] destination buffer
                 , const BufferPtr &source ///< [in] source buffer
                 , std::size_t      offset ///< [in] source buffer offset (the same for destination)
                 , std::size_t      size ///< [in] transfer size
                 ) const;
    /*!
     * \brief   Transfers CPU buffer into device image
     */
    void Transfer( ImagePtr        &destination ///< destination image
                 , const BufferPtr &source ///< source host buffer
                 );

public:
    Caps caps; //< device capabilities 

    std::uint32_t presentQfamilyIdx  = invalidQindex;
    std::uint32_t graphicsQfamilyIdx = invalidQindex;

    vk::UniqueDevice device;
    vk::Queue submission_q;

    // Swapchain
    vk::SwapchainKHR swapchain = nullptr;
    vk::Extent2D draw_rect;  //< presentation surface dimensions
    std::vector<vk::Image> swapchain_images;

private:
    vk::UniqueInstance instance_;
    vk::PhysicalDevice gpu_; //< selected physical device
    vk::UniqueSurfaceKHR wsiSurface_;

    HWND hWnd_;

    vk::UniqueCommandPool cmd_pool_;
    VmaAllocator allocator_;
    std::vector<vk::UniqueCommandBuffer> ctrl_cmd_buf_; //< device control commands
};

extern Hw hw;

#endif // DEVICE_DEVICE_H_
