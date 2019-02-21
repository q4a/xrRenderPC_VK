#include "factory/font/render.h"

#include "frontend/render.h"

/**
 *
 */
void
fFontRender::Initialize
        ( LPCSTR shader
        , LPCSTR texture
        )
{
    frontend.resources_->CreateShader(shader, texture, "", "");
}


/**
 *
 */
void
fFontRender::OnRender
        ( CGameFont &owner
        )
{
    // TBI
}
