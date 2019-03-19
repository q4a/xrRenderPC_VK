/*!
 * \file    ui_render.h
 * \brief   UI renderer interface implementation
 */

#ifndef UI_RENDER_H_
#define UI_RENDER_H_

#include "Include/xrRender/UIRender.h"

class UiRender
    : public IUIRender
{
    /* Render state
     */
    ePrimitiveType primitive_type_ = ptNone; ///< Pipeline primitive topology
    ePointType     point_type_     = pttNone; ///< Vertex format

    std::uint32_t  vertices_count_      = 0; ///< Expected vertices count
    std::size_t    start_vertex_offset_ = 0; ///< Initial vertex buffer offset

public:
    UiRender() = default;
    ~UiRender() = default;

    void CreateUIGeom() final;
    void DestroyUIGeom() final;

    LPCSTR UpdateShaderName(LPCSTR texture, LPCSTR shader) final;
    void SetShader(IUIShader &shader) final;
    void SetAlphaRef(int alpha_reference) final;
    void SetScissor(Irect *area = nullptr) final;

    void GetActiveTextureResolution(Fvector2 &dimensions) final;
    void PushPoint(float x, float y, float z, u32 color, float u, float v) final;
    void StartPrimitive(u32 max_vertices, ePrimitiveType primitive_type,
        ePointType point_type) final;
    void FlushPrimitive() final;

    void CacheSetXformWorld(const Fmatrix& matrix) final;
    void CacheSetCullMode(CullMode cull_mode) final;
};

extern UiRender ui;

#endif // UI_RENDER_H_
