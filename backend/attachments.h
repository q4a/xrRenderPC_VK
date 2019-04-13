#ifndef BACKEND_ATTACHMENTS_H_
#define BACKEND_ATTACHMENTS_H_

#include "device/memory.h"

struct PassAttachment
{
    vk::Format    format;
    vk::Image     image;
    vk::ImageView view;
    bool          need_to_clear  = true;
    bool          from_swapchain = false;
    bool          is_depth       = false;


};


struct SwapchainAttachment
    : public PassAttachment
{
    explicit SwapchainAttachment(vk::Image swapchain_image);
};


class ColorAttachment
    : public PassAttachment
{
    ImagePtr gpu_image_;
public:
    ColorAttachment( std::uint32_t width
                   , std::uint32_t height
                   );
};


class DepthAttachment
    : public PassAttachment
{
    ImagePtr gpu_image_;
public:
    DepthAttachment( std::uint32_t width
                   , std::uint32_t height
                   );
};

#endif // BACKEND_ATTACHMENTS_H_
