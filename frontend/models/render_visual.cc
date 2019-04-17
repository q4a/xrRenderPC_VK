#include "frontend/models/render_visual.h"


vis_data &
RenderVisual::getVisData()
{
    return visibility_data_;
}

u32
RenderVisual::getType()
{
    return type_;
}

#ifdef DEBUG
shared_str
RenderVisual::getDebugName()
{
    return debug__name_;
}
#endif

RenderVisual::RenderVisual()
{
}

RenderVisual::~RenderVisual()
{
}

void
RenderVisual::Load( const char *name, IReader *data, u32 )
{
}

void
RenderVisual::Release()
{
}

void
RenderVisual::Copy
        ( RenderVisual *source
        )
{
    type_            = source->type_;
    visibility_data_ = source->visibility_data_;
    //
}
