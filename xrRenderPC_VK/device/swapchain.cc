#include "device/device.h"


/**
 *
 */
void
Hw::CreateSurface
        ( SDL_Window * const sdlWnd
        )
{
    // check whether our device supports presentation mode
    const auto deviceExtensions = gpu_.enumerateDeviceExtensionProperties();
    auto predicate =
        [](const auto e)
        {
            return !strcmp(e.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        };
    const auto extension =
        std::find_if( cbegin(deviceExtensions)
                    , cend(deviceExtensions)
                    , predicate
        );
    R_ASSERT2(extension != deviceExtensions.end()
        , "Selected device doesn't support swapchain extension"
    );

#if defined(WIN32)
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);

    if (SDL_GetWindowWMInfo(sdlWnd, &info) == SDL_FALSE)
    {
        FATAL_F("! Couldn't get window information: %s", SDL_GetError());
    }

    hWnd_ = info.info.win.window;

    // request a surface from WSI
    const auto surfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
        .setHinstance(::GetModuleHandleA(nullptr))
        .setHwnd(hWnd_);

    wsiSurface_ = instance_->createWin32SurfaceKHRUnique(surfaceCreateInfo);
#else
    static_assert("Only Windows platform supported");
#endif

    // check if GPU can support presentation to the surface
    const auto deviceQueueFamilies = gpu_.getQueueFamilyProperties();
    for ( std::uint32_t queueIdx = 0
        ; queueIdx < deviceQueueFamilies.size()
        ; queueIdx++
        )
    {
        const bool hasSupport =
            gpu_.getSurfaceSupportKHR(queueIdx, wsiSurface_.get());
        if (hasSupport)
        {
            presentQfamilyIdx = queueIdx;
            break;
        }
    }
    R_ASSERT2(presentQfamilyIdx != invalidQindex
        , "Selected device doesn't support given surface"
    );

    // check if the device can work with given WSI
#if defined(WIN32)
    const bool wsiSupported =
        gpu_.getWin32PresentationSupportKHR(presentQfamilyIdx);
    R_ASSERT2(wsiSupported, "Selected device can't work with current WSI");
#endif

    // obtain surface properties
    caps.wsiSurfaceCaps = gpu_.getSurfaceCapabilitiesKHR(wsiSurface_.get());
    caps.wsiSurfacePresentModes =
        gpu_.getSurfacePresentModesKHR(wsiSurface_.get());
    caps.wsiSurfaceFormats = gpu_.getSurfaceFormatsKHR(wsiSurface_.get());

    // Check for required surface capabilites
    const auto& supportedFlags = caps.wsiSurfaceCaps.supportedUsageFlags;
    R_ASSERT2( supportedFlags & vk::ImageUsageFlagBits::eTransferSrc
             , "Swapchain surface can't be used as source image");
    R_ASSERT2( supportedFlags & vk::ImageUsageFlagBits::eTransferDst
             , "Swapchain surface can't be used as target image");
}


/**
 *
 */
vk::PresentModeKHR
Hw::SelectPresentMode() const
{
    auto selectedMode = vk::PresentModeKHR::eMailbox;
    const auto& supportedModes = caps.wsiSurfacePresentModes;
    auto predicate =
        [&, selectedMode](const auto& mode)
        {
            return (mode == selectedMode);
        };
 
    const auto modeIt =
        std::find_if( supportedModes.cbegin()
                    , supportedModes.cend()
                    , predicate
        );

    const bool isMailboxSupported = (modeIt != supportedModes.cend());

    if (   psDeviceFlags.test(rsVSync)
        || !isMailboxSupported
       )
    {
        // FIFO can be used as fallback since its
        // support is mandatory by spec
        selectedMode = vk::PresentModeKHR::eFifo;
    }

    return selectedMode;
}


/**
 *
 */
vk::Format
Hw::SelectColorFormat() const
{
    // TODO: Need to be selected in accordance to the `psCurrentBPP`
    // in fullscreen mode. The R1 preffered opaque XRGB32 so we can use
    // the same strategy. Now it's just RGBA32. Of course need to check
    // if supported.
    return vk::Format::eB8G8R8A8Unorm;
}


/**
 *
 */
vk::ColorSpaceKHR
Hw::SelectColorSpace() const
{
    // TODO: Choose one. Not sure which.
    return vk::ColorSpaceKHR::eSrgbNonlinear;
}


/**
 *
 */
vk::CompositeAlphaFlagBitsKHR
Hw::SelectCompositeAlpha() const
{
    auto selectedAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

    const auto& supportedAlpha = caps.wsiSurfaceCaps.supportedCompositeAlpha;
    R_ASSERT2( (supportedAlpha & selectedAlpha)
             , "No suitable alpha blending mode from WSI"
    ); // TOOD: really it is?

    return selectedAlpha;
}


/**
 *
 */
void
Hw::CreateSwapchain()
{
    const std::uint32_t undefinedSize = std::numeric_limits<std::uint32_t>::max();

    if (caps.wsiSurfaceCaps.currentExtent.width == undefinedSize)
    {
        // surface doesn't care about size. Set desired
        drawRect.setWidth(Device.dwWidth);
        drawRect.setHeight(Device.dwHeight);
    }
    else
    {
        // fixed size: likely windowed mode. Go with it
        // TODO: what should we do? How to notify application?
        drawRect = caps.wsiSurfaceCaps.currentExtent;
    }

    const auto presentMode = SelectPresentMode();
    const auto colorFormat = SelectColorFormat();
    const auto colorSpace = SelectColorSpace();
    const auto alphaMode = SelectCompositeAlpha();
    const auto minImages = caps.wsiSurfaceCaps.minImageCount;
    const auto usageFlags = vk::ImageUsageFlagBits::eColorAttachment
                          | vk::ImageUsageFlagBits::eTransferSrc
                          | vk::ImageUsageFlagBits::eTransferDst;

    const auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR()
        .setOldSwapchain(swapchain)
        .setSurface(wsiSurface_.get())
        .setImageExtent(drawRect)
        .setPresentMode(presentMode)
        .setMinImageCount(minImages)
        .setImageFormat(colorFormat)
        .setImageColorSpace(colorSpace)
        .setCompositeAlpha(alphaMode)
        .setImageUsage(usageFlags)
        .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
        .setImageArrayLayers(1);

    swapchain = device->createSwapchainKHR(swapchainCreateInfo);

    // retrieve swapchain images and create views for them
    const auto swapchainImages =
        device->getSwapchainImagesKHR(swapchain);

    baseRt.clear();
    baseRt.reserve(swapchainImages.size());

    auto process =
        [&, colorFormat](const auto& image)
        {
            const auto subresourceRange =
                vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor
                                         , 0
                                         , 1 
                                         , 0
                                         , 1
                );
        
            const auto imageViewCreateInfo = vk::ImageViewCreateInfo()
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(colorFormat)
                .setImage(image)
                .setSubresourceRange(subresourceRange);

            SwapchainResource resource;
            resource.image = image;
            resource.imageView =
                device->createImageView(imageViewCreateInfo);

            baseRt.push_back(resource);
        };

    std::for_each( swapchainImages.cbegin()
                 , swapchainImages.cend()
                 , process
    );

    caps.colorFormat = colorFormat;
}


/**
 *
 */
void
Hw::DestroySwapchain()
{
    for (auto& resource : baseRt)
    {
        device->destroyFramebuffer(resource.frameBuffer);
        device->destroyImageView(resource.imageView);
    }
    device->destroySwapchainKHR(swapchain);
}
