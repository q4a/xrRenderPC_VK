#include "device/device.h"

#include "backend/attachments.h"


//-----------------------------------------------------------------------------
SwapchainAttachment::SwapchainAttachment
        ( vk::Image swapchain_image
        )
{
    format         = hw.caps.colorFormat;
    image          = swapchain_image;
    need_to_clear  = psDeviceFlags.test(rsClearBB);
    from_swapchain = true;

    const auto subresource_range =
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor
                                    , 0
                                    , 1 
                                    , 0
                                    , 1
        );
        
    const auto view_create_info = vk::ImageViewCreateInfo()
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(hw.caps.colorFormat)
        .setImage(image)
        .setSubresourceRange(subresource_range);

    view = hw.device->createImageView(view_create_info);
}


//-----------------------------------------------------------------------------
DepthAttachment::DepthAttachment
        ( std::uint32_t width
        , std::uint32_t height
        )
{
    format        = hw.caps.depthFormat;
    need_to_clear = true;
    is_depth      = true;
    gpu_image_    = hw.CreateGpuImage( { width , height, 1 }
                                     , format
                                     , 1
                                     , 1
                                     , ImageType::Depth
    );
    image = gpu_image_->image;
    view  = gpu_image_->CreateView();
}


//-----------------------------------------------------------------------------
ColorAttachment::ColorAttachment
        ( std::uint32_t width
        , std::uint32_t height
        )
{
    format     = vk::Format::eB8G8R8A8Unorm;
    gpu_image_ = hw.CreateGpuImage( { width, height ,1 }
                                  , format
                                  , 1
                                  , 1
                                  , ImageType::Buffer
    );
    image = gpu_image_->image;
    view  = gpu_image_->CreateView();
}
