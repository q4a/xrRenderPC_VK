#include "resources/texture_desc.h"


//-----------------------------------------------------------------------------
void
TextureDescription::Load()
{
    LoadThm("$game_textures$");
    LoadThm("$level$");
}


//-----------------------------------------------------------------------------
void
TextureDescription::Unload()
{
    // TBI
}


//-----------------------------------------------------------------------------
bool
TextureDescription::GetDetailTexture
        ( const std::string     &texture_name
        , std::string           &detail_name
        , std::function<void()> &detail_scaler
        )
{
    // TBI
    return false;
}

void
TextureDescription::LoadThm
        ( const std::string &mount_point
        )
{
    // TBI
}
