/*!
 * \file    render_target.cc
 * \brief   Render targets manager implementation
 */

#include "backend/backend.h"
#include "device/device.h"
#include "resources/manager.h"

#include "frontend/render_target.h"


//-----------------------------------------------------------------------------
PassDescription::PassDescription
        ( std::array<std::shared_ptr<PassAttachment>, 4> attachments
        )
{
    std::vector<vk::AttachmentDescription> attachment_descriptions;
    std::vector<vk::AttachmentReference>   color_attachment_references;
    std::vector<vk::AttachmentReference>   depth_attachment_references;
    std::vector<vk::ImageView>             attachment_views;

    /* Attachments
     */
    for (const auto &attachment : attachments)
    {
        if (!attachment)
        {
            continue;
        }

        const auto load_operation =
            attachment->need_to_clear
                ? vk::AttachmentLoadOp::eClear
                : vk::AttachmentLoadOp::eDontCare;

        const auto layout =
            attachment->is_depth
                ? vk::ImageLayout::eDepthStencilAttachmentOptimal
                : vk::ImageLayout::eColorAttachmentOptimal;

        const auto final_layout =
            attachment->is_depth
                ? vk::ImageLayout::eDepthStencilAttachmentOptimal
                : attachment->from_swapchain
                    ? vk::ImageLayout::ePresentSrcKHR
                    : vk::ImageLayout::eShaderReadOnlyOptimal;

        const auto description = vk::AttachmentDescription()
            .setFormat(attachment->format)
            .setSamples(vk::SampleCountFlagBits::e1) // TODO
            .setLoadOp(load_operation)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(final_layout);

        const auto reference = vk::AttachmentReference()
            .setAttachment(attachment_descriptions.size())
            .setLayout(layout);

        attachment_descriptions.push_back(description);

        if (attachment->is_depth)
        {
            depth_attachment_references.push_back(reference);
        }
        else
        {
            color_attachment_references.push_back(reference);
        }
    }

    /* Subpass
     */
    const auto subpass = vk::SubpassDescription()
        .setPColorAttachments(color_attachment_references.data())
        .setColorAttachmentCount(color_attachment_references.size())
        .setPDepthStencilAttachment(depth_attachment_references.data())
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    std::array<vk::SubpassDependency, 1> subpass_dependencies;
    subpass_dependencies[0]
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead
            | vk::AccessFlagBits::eColorAttachmentWrite
        );

    /* Render pass
     */
    const auto renderpass_create_info = vk::RenderPassCreateInfo()
        .setAttachmentCount(attachment_descriptions.size())
        .setPAttachments(attachment_descriptions.data())
        .setSubpassCount(1)
        .setPSubpasses(&subpass)
        .setDependencyCount(subpass_dependencies.size())
        .setPDependencies(subpass_dependencies.data());

    render_pass = hw.device->createRenderPass(renderpass_create_info);
}


//-----------------------------------------------------------------------------
void
PassDescription::CreateFrameBuffer
        ( const std::vector<vk::ImageView> &views
        )
{
    const auto framebuffer_create_info = vk::FramebufferCreateInfo()
        .setRenderPass(render_pass)
        .setAttachmentCount(views.size())
        .setPAttachments(views.data())
        .setWidth(hw.draw_rect.width)
        .setHeight(hw.draw_rect.height)
        .setLayers(1); // not stereo

    auto frame_buffer =
        hw.device->createFramebufferUnique(framebuffer_create_info);

    frame_buffers.push_back(std::move(frame_buffer));
}


//-----------------------------------------------------------------------------
RenderTarget::RenderTarget()
    : width(hw.draw_rect.width)
    , height(hw.draw_rect.height)
{
    CreateBase();
    CreateGeneric0();
    CreateGeneric1();
}


//-----------------------------------------------------------------------------
void
RenderTarget::CreateBase()
{
    // Depth buffer
    default_depth = std::make_shared<DepthAttachment>(width, height);

    // Swapchain images
    for (auto image : hw.swapchain_images)
    {
        auto default_color = std::make_shared<SwapchainAttachment>(image);
        default_colors.push_back(default_color);
    }

    auto description = std::unique_ptr<PassDescription>{
    new PassDescription{ { default_colors[0] // used for reference
                         , {}
                         , {}
                         , default_depth
                         }
    } };

    // Framebuffers
    for (const auto &default_color : default_colors)
    {
        description->CreateFrameBuffer( { default_color->view
                                        , default_depth->view
        });
    }

    render_passes.insert(std::make_pair( Type::Base
                                       , std::move(description)
    ));
}


//-----------------------------------------------------------------------------
void
RenderTarget::CreateGeneric0()
{
    generic0_color = std::make_shared<ColorAttachment>(width, height);

    auto &texture = resources.CreateTexture("$user$generic0");
    texture->view = generic0_color->view;

    auto description = std::unique_ptr<PassDescription>{
        new PassDescription{ { generic0_color
                             , {}
                             , {}
                             , default_depth
                             }
    } };

    description->CreateFrameBuffer( { generic0_color->view
                                    , default_depth->view
    });

    render_passes.insert(std::make_pair( Type::Generic0
                                       , std::move(description)
    ));
}


//-----------------------------------------------------------------------------
void
RenderTarget::CreateGeneric1()
{
    generic1_color = std::make_shared<ColorAttachment>(width, height);

    auto &texture = resources.CreateTexture("$user$generic1");
    texture->view = generic1_color->view;

    auto description = std::unique_ptr<PassDescription>{
        new PassDescription{ { generic1_color
                             , {}
                             , {}
                             , default_depth
                             }
    } };

    description->CreateFrameBuffer( { generic1_color->view
                                    , default_depth->view
                                    }
    );

    render_passes.insert(std::make_pair( Type::Generic1
                                       , std::move(description)
    ));
}


//-----------------------------------------------------------------------------
u32
RenderTarget::get_width()
{
    return hw.draw_rect.width;
}


//-----------------------------------------------------------------------------
u32
RenderTarget::get_height()
{
    return hw.draw_rect.height;
}

//-----------------------------------------------------------------------------
void
RenderTarget::Set
        ( Type type
        )
{
    const auto &iterator = render_passes.find(type);
    VERIFY(iterator != render_passes.cend());

    backend.SetRenderPass(*iterator->second.get());
}
