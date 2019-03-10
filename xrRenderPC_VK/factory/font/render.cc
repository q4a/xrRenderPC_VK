#include "factory/font/render.h"

#include "resources/manager.h"

/**
 *
 */
void
fFontRender::Initialize
        ( LPCSTR shader_name
        , LPCSTR texture_name
        )
{
    shader_ = resources.CreateShader( shader_name
                                    , texture_name
    );
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
