#ifndef FRONTEND_RENDER_H_
#define FRONTEND_RENDER_H_

#include <memory>
#include <vector>

#include "xrEngine/pure.h"
#include "xrEngine/Render.h"

#include "frontend/legacy.h"
#include "frontend/main_menu.h"
#include "frontend/render_target.h"
#include "resources/manager.h"


class FrontEnd
    : public LegacyInterface
{
    MenuRender main_menu_;

    std::uint32_t current_image_ = 0;
    vk::Result swapchain_state_ = vk::Result::eSuccess;

public:
    FrontEnd() = default;
    ~FrontEnd() = default;

    void OnDeviceDestroy(bool keep_textures) final;
    void DestroyHW() final;
    void OnDeviceCreate(LPCSTR shader) final;
    void Create(SDL_Window* hwnd, u32 &width, u32 &height, float &width_2,
        float &height_2) final;

    DeviceState GetDeviceState() final;
    void Begin() final;
    void End() final;

    void Render() final;
    IRender_Target* getTarget() final;

    struct RenderOptions
    {
        int  shadow_map_size            = 2048u; // TODO: take it from console setting
        bool shader__dump_source        = true;
        bool shader__disassemble        = false;
    } o;

    std::shared_ptr<RenderTarget> render_target;
};

extern FrontEnd frontend;

#endif // FRONTEND_RENDER_H_
