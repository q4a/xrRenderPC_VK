/*!
 * \file    render_target.h
 * \brief   Render targets manager interface
 */

#ifndef FRONTEND_RENDER_TARGET_H_
#define FRONTEND_RENDER_TARGET_H_

#include <array>
#include <memory>
#include <unordered_map>

#include "xrEngine/Render.h"

#include "backend/attachments.h"

struct PassDescription
{
    PassDescription( const std::array<std::shared_ptr<PassAttachment>, 4> attachments
                   );
    ~PassDescription() = default; // unique members will be destroyed automatically

    void CreateFrameBuffer(const std::vector<vk::ImageView> &views);

    std::vector<vk::UniqueFramebuffer> frame_buffers;
    vk::RenderPass                     render_pass;
};

class RenderTarget
    : public IRender_Target
{
    void CreateBase();
    void CreateGeneric0();
    void CreateGeneric1();

    std::shared_ptr<DepthAttachment>                  default_depth;
    std::vector<std::shared_ptr<SwapchainAttachment>> default_colors;

    std::shared_ptr<ColorAttachment> generic0_color;
    std::shared_ptr<ColorAttachment> generic1_color;

    const std::uint32_t width;
    const std::uint32_t height;

public:
    RenderTarget();
    ~RenderTarget() = default; // TODO: do we need to destroy smth?

    enum class Type : std::uint32_t
    {
        Base,
        Generic0,
        Generic1
    };

    void Set(Type type);

    std::unordered_map<Type, std::unique_ptr<PassDescription>> render_passes;

    // Interface methods
    void set_blur(float f) override {}
    void set_gray(float f) override {}
    void set_duality_h(float f) override {}
    void set_duality_v(float f) override {}
    void set_noise(float f) override {}
    void set_noise_scale(float f) override {}
    void set_noise_fps(float f) override {}
    void set_color_base(u32 f) override {}
    void set_color_gray(u32 f) override {}
    void set_color_add(const Fvector& f) {}
    /*!
     * \brief   Returns width of render target surface
     */
    u32 get_width() final;
    /*!
     * \brief   Returns height of render target surface
     */
    u32 get_height() final;
    void set_cm_imfluence(float f) override {}
    void set_cm_interpolate(float f) override {}
    void set_cm_textures(const shared_str& tex0, const shared_str& tex1) override {}
};

#endif // FRONTEND_RENDER_TARGET
