#include <vector>

#include <gli\gli.hpp>

#include "device/device.h"
#include "backend/streams.h"
#include "resources/manager.h"

#include "resources/texture.h"


Texture::Texture()
    : Loader(std::bind(&Texture::ApplyLoad, this))
{

}


std::shared_ptr<Texture>
ResourceManager::CreateTexture
        ( const std::string &name
        )
{
    if (name == "null")
    {
        return {};
    }

    // Check if we already have this resource created
    const auto &iterator = textures_.find(name);
    if (iterator != textures_.cend())
    {
        return iterator->second;
    }

    auto texture = std::make_shared<Texture>();
    texture->set_name(name.c_str());
    texture->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    textures_.insert(std::make_pair(name, texture));

    if (/*device ready &&*/!resources.deferred_load)
    {
        texture->Load();
    }

    return texture;
}


void
Texture::ApplyTheora()
{
    // TBI
    R_ASSERT(false);
}


void
Texture::ApplyAvi()
{
    // TBI
    R_ASSERT(false);
}


void
Texture::ApplySequence()
{
    // TBI
    R_ASSERT(false);
}


void
Texture::ApplyNormal()
{
    if (flags.staging)
    {
        image->Sync();
    }
}


void
Texture::ApplyLoad()
{
    if (!flags.loaded)
    {
        Load();
    }
    else
    {
        // Select loader
        Postload();
    }

    // Bypass to loader
    Loader();
}


void
Texture::Preload()
{
    // TBI
}


void
Texture::Postload()
{
    R_ASSERT(type != TextureType::Unknown);

    switch (type)
    {
    case TextureType::Theora:
        Loader = std::bind(&Texture::ApplyTheora, this);
        break;
    case TextureType::Avi:
        Loader = std::bind(&Texture::ApplyAvi, this);
        break;
    case TextureType::Sequence:
        Loader = std::bind(&Texture::ApplySequence, this);
        break;
    case TextureType::Normal:
        // no break
    default:
        Loader = std::bind(&Texture::ApplyNormal, this);
        break;
    }
}


void
Texture::Load()
{
    R_ASSERT(!image);

    const std::string name{ *cName };

    if (name == "$null")
    {
        return;
    }

    if (name == "$user")
    {
        flags.user = true;
        return;
    }

    Preload();

    /* Determine resource type by extension
     */
    string_path path;

    if (FS.exist( path
                , "$game_textures$"
                , name.c_str()
                , ".ogm"
       ))
    {
        /* Theora video stream
         */

        type = TextureType::Theora;
        R_ASSERT2(false, "Theora stream isn't supported yet");
    }
    else if (FS.exist( path
                     , "$game_textures$"
                     , name.c_str()
                     , ".avi"
            ))
    {
        /* AVI stream
         */

        type = TextureType::Avi;
        R_ASSERT2(false, "AVI stream isn't supported yet");
    }
    else if (FS.exist( path
                     , "$game_textures$"
                     , name.c_str()
                     , ".seq"
            ))
    {
        /* Animation sequence
         */

        type = TextureType::Sequence;
        R_ASSERT2(false, "Animation sequence isn't supported yet");
    }
    else
    {
        /* Normal texture
         */

        flags.staging = true; // use staging by default
        TextureLoad(name);

        type = TextureType::Normal;
    }

    flags.loaded = true;

    Postload();
}


constexpr LPCSTR fallback_texture_name = "ed\\ed_not_existing_texture.dds";


void
Texture::TextureLoad
        ( const std::string &name
        )
{
    // TODO: check for _bump

    string_path path;
    const std::vector<LPCSTR> root_directories =
    {
        "$game_textures$",
        "$level$",
        "$game_saves$"
    };

    bool was_found = false;
    for (const auto &root : root_directories)
    {
        if (FS.exist( path
                    , root
                    , name.c_str()
                    , ".dds"
           ))
        {
            was_found = true;
            break;
        }
    }

    if (!was_found)
    {
        Msg("! Can't find texture '%s'", name.c_str());
        // Open fallback texture
        bool result = FS.exist( path
                              , "$game_textures$"
                              , fallback_texture_name
        );
        R_ASSERT2(result, "Unable to load fallback texture");
    }

    auto rstream = FS.r_open(path);
    R_ASSERT(rstream);

    image =
        std::shared_ptr<StreamImage>{ new StreamImage{ rstream->pointer()
                                                     , (size_t)rstream->length()
                                                     }
        };

    if (!flags.staging)
    {
        // If no staging option specified, load the image immediately.
        // Otherwise loading will be deferred until texture applied.
        image->Sync();
    }

    // TODO: 3D, Cubemaps
    // TODO: LODs

    Msg( "* Loaded: %s[%d]"
       , name.c_str()
       , rstream->length()
    );

    FS.r_close(rstream);
}
