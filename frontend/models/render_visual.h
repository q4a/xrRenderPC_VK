/*
 */

#ifndef FRONTEND_RENDER_VISUAL_H_
#define FRONTEND_RENDER_VISUAL_H_

#include "Include/xrRender/RenderVisual.h"

class RenderVisual
    : public IRenderVisual
{
    friend class ModelPool;

public:
    RenderVisual();
    virtual ~RenderVisual();

    vis_data &getVisData();
    u32 getType();
#ifdef DEBUG
    shared_str getDebugName();
#endif

    virtual void Render(float lod) {}; // LOD - Level Of Detail  [0..1], Ignored
    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Release(); // Shared memory release
    virtual void Copy(RenderVisual *from);
    virtual void Spawn(){};
    virtual void Depart(){};

private:
    std::uint32_t type_; ///< type of visual
    vis_data      visibility_data_;
    //ref_shader shader; // pipe state, shared
#ifdef DEBUG
    shared_str    debug__name_;
#endif
};

#endif // FRONTEND_RENDER_VISUAL_H_
