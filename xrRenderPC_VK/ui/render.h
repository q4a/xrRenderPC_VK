#ifndef UI_RENDER_H_
#define UI_RENDER_H_

#include "common.h"

#include "Include/xrRender/UIRender.h"

class UiRender
    : public IUIRender
{
public:
    UiRender() = default;
    ~UiRender() = default;

    void CreateUIGeom() override {} // TBI
    void DestroyUIGeom() override {} // TBI

    void SetShader(IUIShader& shader) override {} // TBI
    void SetAlphaRef(int aref) override {} // TBI
    void SetScissor(Irect* rect = nullptr) override {} // TBI
    void GetActiveTextureResolution(Fvector2& res) override {} // TBI
    void PushPoint(float x, float y, float z, u32 C, float u, float v) override {} // TBI
    void StartPrimitive(u32 max_vertices, ePrimitiveType primitive_type,
        ePointType point_type) override {} // TBI
    void FlushPrimitive() override {} // TBI
    LPCSTR UpdateShaderName(LPCSTR texture, LPCSTR shader) override { return ""; } // TBI
    void CacheSetXformWorld(const Fmatrix& M) override {} // TBI
    void CacheSetCullMode(CullMode mode) override {} // TBI
};

extern UiRender ui;

#endif // UI_RENDER_H_
