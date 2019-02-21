#include <functional>
#include <string>

#include "Layers/xrRender/blenders/Blender_CLSID.h"

#include "resources/manager.h"
#include "resources/blender_compiler.h"
#include "resources/blender_screen_set.h"


/**
 *
 */
std::shared_ptr<Constant>
ResourceManager::CreateConstant
        ( LPCSTR name
        )
{
    R_ASSERT(name && name[0]);

    if (xr_stricmp(name, "$null") == 0)
    {
        return nullptr;
    }

    const auto &iterator = constants_.find(name);
    if (iterator != constants_.end())
    {
        return iterator->second;
    }

    auto c = std::make_shared<Constant>();
    c->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    c->dwReference = 1;
    constants_.insert(std::make_pair(c->set_name(name), c));
    return c;
}


/**
 *
 */
void
ResourceManager::DeleteConstant
        ( const std::shared_ptr<Constant> &constant
        )
{
    if ((constant->dwFlags & xr_resource_flagged::RF_REGISTERED) == 0)
    {
        return;
    }

    const auto name = *constant->cName;
    auto &iterator = constants_.find(name);

    if (iterator != constants_.end())
    {
        constants_.erase(iterator);
        return;
    }
    Msg("! ERROR: Unable to find constant '%s'", name);
}


/**
 *
 */
std::shared_ptr<Matrix>
ResourceManager::CreateMatrix
        ( LPCSTR name
        )
{
    R_ASSERT(name && name[0]);

    if (xr_stricmp(name, "$null") == 0)
    {
        return nullptr;
    }

    const auto &iterator = matrices_.find(name);
    if (iterator != matrices_.end())
    {
        return iterator->second;
    }

    auto m = std::make_shared<Matrix>();
    m->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    m->dwReference = 1;
    matrices_.insert(std::make_pair(m->set_name(name), m));
    return m;
}


void
ResourceManager::DeleteMatrix
        ( const std::shared_ptr<Matrix> &matrix
        )
{
    if ((matrix->dwFlags & xr_resource_flagged::RF_REGISTERED) == 0)
    {
        return;
    }

    const auto name = *matrix->cName;
    auto &iterator = matrices_.find(name);

    if (iterator != matrices_.end())
    {
        matrices_.erase(iterator);
        return;
    }
    Msg("! ERROR: Unable to find matrix '%s'", name);
}


/**
 *
 */
std::shared_ptr<Blender>
ResourceManager::CreateBlender
        ( CLASS_ID class_id
        )
{
    std::shared_ptr<Blender> blender{};

    switch (class_id)
    {
    case B_SCREEN_SET:
        blender = std::make_shared<blenders::ScreenSet>();
        break;

    default:
        // nothing here
        break;
    };

    return blender;
}


/**
 *
 */
std::shared_ptr<Blender>
ResourceManager::GetBlender
        ( const std::string &name
        )
{
    R_ASSERT(name.size());
    const auto &iterator = blenders_.find(name);

    if (iterator == blenders_.end())
    {
        xrDebug::Fatal(DEBUG_INFO, "Blender '%s' not found in library.", name);
        return nullptr;
    }

    return iterator->second;
}


enum ShaderBaseChunkType
{
    eConstant,
    eMatrix,
    eBlender
};


/**
 *
 */
void
ResourceManager::OnDeviceCreate
        ( const std::string &file_name
        )
{
    const std::string signature { "shENGINE" };
    string32 id;
    IReader *rstream = FS.r_open(file_name.c_str());
    
    R_ASSERT2(rstream, file_name);
    
    rstream->r(&id, signature.size());
    if (strncmp(id, signature.c_str(), signature.size()) == 0)
    {
        FATAL("Unsupported blender library. Compressed?");
    }
    
    // Load constants
    auto fs = rstream->open_chunk(eConstant);
    if (fs)
    {
        string256 name;

        while (!fs->eof())
        {
            fs->r_stringZ(name, sizeof(name));
            auto c = CreateConstant(name);
            c->Load(fs);
        }
        fs->close();
    }

    // Load matrices
    fs = rstream->open_chunk(eMatrix);
    if (fs)
    {
        string256 name;

        while (!fs->eof())
        {
            fs->r_stringZ(name, sizeof(name));
            auto m = CreateMatrix(name);
            m->Load(fs);
        }
        fs->close();
    }

    // Load blenders
    fs = rstream->open_chunk(eBlender);
    if (fs)
    {
        IReader *chunk;
        int chunk_id = 0;

        while ((chunk = fs->open_chunk(chunk_id)))
        {
            BlenderDescription desc;
            chunk->r(&desc, sizeof (desc));

            auto b = CreateBlender(desc.class_id);
            
            if (b)
            {
                if (b->GetDescription().version != desc.version)
                {
                    Msg("! Version conflict in blender '%s'", desc.name);
                }

                chunk->seek(0);
                b->Load(*chunk, desc.version);

                std::string name{ desc.name };
                auto &iterator = blenders_.insert(std::make_pair(name, b));
                R_ASSERT2(iterator.second, "shader.xr - found duplicate name");
            }
            else
            {
                Msg("! Renderer doesn't support blender '%s'", desc.name);
            }

            chunk->close();
            chunk_id++;
        }
        fs->close();
    }

    FS.r_close(rstream);

    texture_description_.Load();
}


/**
 *
 */
void
ResourceManager::OnDeviceDestroy()
{
    texture_description_.Unload();

    // Destroy blenders
    for (auto&& blender : blenders_)
    {
        R_ASSERT(blender.second.unique());
    }
    blenders_.clear();

    // Destroy matrices
    for (auto&& matrix : matrices_)
    {
        R_ASSERT(matrix.second.unique());
    }
    matrices_.clear();

    // Destroy constants
    for (auto&& constant : constants_)
    {
        R_ASSERT(constant.second.unique());
    }
    constants_.clear();
}


const std::vector<std::string> texture_extensions =
{
    "tga",
    "dds",
    "bmp",
    "ogm"
};


/**
 *
 */
void
ResourceManager::ParseList
        ( const std::string &list
        , std::vector<std::string> &strings
        )
{
    strings.clear();

    if (list.size() == 0)
    {
        strings.push_back("$null");
        return;
    }

    // Split string
    std::function<void ( const std::string&
                       , std::vector<std::string>&
                       , const char
    )> split_with;

    split_with =
        [&]( const std::string &list
           , std::vector<std::string> &strings
           , const char delimiter
           )
    {
        if (list.size() == 0)
        {
            return;
        }

        auto &iterator = std::find(list.cbegin(), list.cend(), delimiter);
        strings.push_back({ list.cbegin(), iterator });

        if (iterator != list.cend())
        {
            split_with({ ++iterator, list.cend() }, strings, delimiter);
        }
    };

    split_with(list, strings, ',');

    // Remove extensions
    auto remove_extension = [](const std::string &file_name) -> std::string
    {
        for (const auto &extension : texture_extensions)
        {
            auto &iterator = std::find_end( file_name.cbegin()
                                          , file_name.cend()
                                          , extension.cbegin()
                                          , extension.cend()
            );

            if (iterator != file_name.cend())
            {
                return { file_name.cbegin(), --iterator };
            }
        }

        return file_name;
    };

    std::transform( strings.cbegin()
                  , strings.cend()
                  , strings.begin()
                  , remove_extension
    );
}


/**
 *
 */
std::shared_ptr<Shader>
ResourceManager::CreateShader
        ( const std::string &shader_name
        , const std::string &textures
        , const std::string &constants
        , const std::string &matrices
        )
{
    std::shared_ptr<Shader> shader;

    // TODO: check for LUA shaders
    auto blender = GetBlender(shader_name);
    if (!blender)
    {
        return shader; // null
    }

    shader = std::make_shared<Shader>();

    // Initialize compiler
    BlenderCompiler compiler;
    ParseList(textures,  compiler.textures);
    ParseList(constants, compiler.constants);
    ParseList(matrices,  compiler.matrices);

    compiler.blender = blender;
    compiler.detail  = false; // TODO

    // Compile

    // LOD0 - HQ
    auto element = Element{};
    compiler.Compile(element);
    shader->elements[0] = std::make_shared<Element>(element);
    elements_.push_back(shader->elements[0]);

    // TODO: other passes

    // TODO: check for unique
    shaders_.push_back(shader);
    return shader;
}
