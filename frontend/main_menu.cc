/*!
 * \file    main_menu.cc
 * \brief   Main menu render implementation
 */


#include "xrCore/math_constants.h"
#include "xrGame/GamePersistent.h"

#include "backend/backend.h"
#include "backend/vertex_formats.h"
#include "frontend/render.h"
#include "resources/manager.h"

#include "frontend/main_menu.h"


void
MenuRender::Create()
{
    shader_ = resources.CreateShader("distort", "null");
    VERIFY(shader_);
}


//-----------------------------------------------------------------------------
void
MenuRender::Destroy()
{
    VERIFY(shader_);
    resources.DestroyShader(shader_);
}


//-----------------------------------------------------------------------------
void
MenuRender::Render()
{
    // Main menu render
    frontend.render_target->Set(RenderTarget::Type::Generic0);
    g_pGamePersistent->OnRenderPPUI_main();

    // Post processing
    const std::array<float, 4> clear_color =
    {
        127.0f / 255.0f,    // R
        127.0f / 255.0f,    // G
                   0.0f,    // B
        127.0f / 255.0f     // A
    };
    backend.SetClearColor(clear_color);
    frontend.render_target->Set(RenderTarget::Type::Generic1);
    g_pGamePersistent->OnRenderPPUI_PP();

    // Display
    frontend.render_target->Set(RenderTarget::Type::Base);

    auto &vertex_buffer = backend.vertex_stream;
    std::size_t offset_start = 0;
    vertex_buffer >> offset_start;

    vertex_format::TriangleList vertex;

    // Original code used here
    Fvector2 p0, p1;
    u32 Offset;
    auto C = glm::vec4{ 1.0f };
    float _w = float(Device.dwWidth);
    float _h = float(Device.dwHeight);
    float d_Z = EPS_S;
    float d_W = 1.f;
    p0.set(.5f / _w, .5f / _h);
    p1.set((_w + .5f) / _w, (_h + .5f) / _h);

    vertex.Set(glm::vec4{ EPS, float(_h + EPS), d_Z, d_W }, glm::vec2{ p0.x, p1.y }, glm::vec4{ 1.0f });
    vertex_buffer << (BufferStride)vertex;
    vertex.Set(glm::vec4{ EPS, EPS, d_Z, d_W }, glm::vec2{ p0.x, p0.y }, glm::vec4{ 1.0f });
    vertex_buffer << (BufferStride)vertex;
    vertex.Set(glm::vec4{ float(_w + EPS), float(_h + EPS), d_Z, d_W }, glm::vec2{ p1.x, p1.y }, glm::vec4{ 1.0f });
    vertex_buffer << (BufferStride)vertex;
    vertex.Set(glm::vec4{ float(_w + EPS), EPS, d_Z, d_W }, glm::vec2{ p1.x, p0.y }, glm::vec4{ 1.0f });
    vertex_buffer << (BufferStride)vertex;

    backend.SetShader(shader_);
    backend.SetGeometry(vertex_buffer, backend.index_cache);

    std::size_t offset_end = 0;
    vertex_buffer >> offset_end;

    const auto num_vertices =
        (offset_end - offset_start) / vertex_format::TriangleList::size;
    const auto num_primitives = num_vertices / triangles_per_quad;

    backend.DrawIndexed(num_primitives);
}
