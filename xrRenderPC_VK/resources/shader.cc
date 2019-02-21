#include <shaderc/shaderc.hpp>

#include "device/device.h"
#include "frontend/render.h"

#include "resources/manager.h"

#pragma comment(lib, "shaderc_combined")

/**
 *
 */
template <>
VertexShaders &
ResourceManager::GetShaderMap()
{
    return vertex_shaders_;
}


/**
 *
 */
template <>
FragmentShaders &
ResourceManager::GetShaderMap()
{
    return fragment_shaders_;
}


/**
 *
 */
std::shared_ptr<VertexShader>
ResourceManager::CreateVertexShader
        ( const std::string &name
        )
{
    auto shader =
        CreatePipelineShader<VertexShader>( name
                                          , name
                                          , true
    );
    return shader;
}


/**
 *
 */
std::shared_ptr<FragmentShader>
ResourceManager::CreateFragmentShader
        ( const std::string &name
        )
{
    return {};
}


/**
 *
 */
template <class T>
std::shared_ptr<T>
ResourceManager::CreatePipelineShader
        ( const std::string &name
        , const std::string &file_name
        , bool s
        )
{
    auto &shader_map = GetShaderMap<class ShaderTypeTraits<T>::MapType>();

    // Check if shader already created
    const auto &iterator = shader_map.find(name);
    if (iterator != shader_map.cend())
    {
        return iterator->second;
    }

    // Create a shader object
    auto shader = std::make_shared<T>();

    shader->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    shader->set_name(name.c_str());
    shader_map.insert(std::make_pair(name, shader));

    // TODO: fix file name

    // Open file
    std::string path = frontend.getShaderPath();
    path += name;
    path += ShaderTypeTraits<T>::file_extension;

    string_path path_expanded;
    FS.update_path(path_expanded, "$game_shaders$", path.c_str());
    IReader *rstream = FS.r_open(path_expanded);

    // TODO: replace by `stub_default`
    R_ASSERT(rstream);

    // TODO: entry point override
    const std::string entry_point = "main";
    bool result = CompileShader(name, rstream, entry_point);
    R_ASSERT(result);

    rstream->close();

    return shader;
}


/**
 *
 */
bool
ResourceManager::CompileShader
        ( const std::string &name
        , IReader *rstream
        , const std::string &entry_point
        )
{
    return true;
}
