#ifndef DEBUG_RENDER_H_
#define DEBUG_RENDER_H_

#include "common.h"

#include "Include/xrRender/DebugRender.h"

class DebugRender
    : public IDebugRender
{
public:
    DebugRender() = default;
    ~DebugRender() = default;

    void Render() override {}; // TBI
    void add_lines(Fvector const* vertices, u32 const& vertex_count,
        u16 const* pairs, u32 const& pair_count, u32 const& color) override {}; // TBI

    void NextSceneMode() override {}; // TBI
    void ZEnable(bool bEnable) override {}; // TBI
    void OnFrameEnd() override {}; // TBI
    void SetShader(const debug_shader& shader) override {}; // TBI
    void CacheSetXformWorld(const Fmatrix& M) override {}; // TBI
    void CacheSetCullMode(CullMode) override {}; // TBI
    void SetAmbient(u32 colour) override {}; // TBI

    // Shaders
    void SetDebugShader(dbgShaderHandle shader_handle) override {}; // TBI
    void DestroyDebugShader(dbgShaderHandle shader_handle) override {}; // TBI

#ifdef DEBUG
    void dbg_DrawTRI(Fmatrix& T, Fvector& p1, Fvector& p2,
        Fvector& p3, u32 C) override {}; // TBI
#endif
};

extern DebugRender debug;

#endif // DEBUG_RENDER_H_
