#include "backend/backend.h"
#include "factory/ui/shader.h"

#include "ui/render.h"

UiRender ui;

LPCSTR
UiRender::UpdateShaderName
        ( LPCSTR texture_name
        , LPCSTR shader_name
        )
{
    string_path path;
    const bool is_exist = FS.exist( path
                                  , "$game_textures$"
                                  , texture_name
                                  , ".ogm"
    );

    if (is_exist)
    {
        return ("hud" DELIMITER "movie");
    }
    else
    {
        return shader_name;
    }
}


void
UiRender::SetShader
        ( IUIShader &shader
        )
{
    const auto &ui_shader = dynamic_cast<fUIShader&>(shader);
    backend.SetShader(ui_shader.shader_);
}
