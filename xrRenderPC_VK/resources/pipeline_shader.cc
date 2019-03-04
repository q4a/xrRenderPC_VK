#include "resources/manager.h"

#include "resources/pipeline_shader.h"


/**
 *
 */
template <>
VertexShaders &
ResourceManager::GetShaderMap<VertexShader>()
{
    return vertex_shaders_;
}


/**
 *
 */
template <>
FragmentShaders &
ResourceManager::GetShaderMap<FragmentShader>()
{
    return fragment_shaders_;
}


/**
 *
 */
VertexShaderPtr
ResourceManager::CreateVertexShader
(const std::string &name
)
{
    std::string shader_name{ name };

    // TODO: check for skinning value

    return CreatePipelineShader<VertexShader>
                ( shader_name
                , name
    );
}


/**
 *
 */
FragmentShaderPtr
ResourceManager::CreateFragmentShader
        ( const std::string &name
        )
{
    std::string shader_name{ name };

    // TODO: check for MS value

    return CreatePipelineShader<FragmentShader>
                ( shader_name
                , name
    );
}


/**
 *
 */
template <class T>
std::shared_ptr<T>
ResourceManager::CreatePipelineShader
        ( const std::string &shader_name
        , const std::string &file_name
        )
{
    auto &shader_map = GetShaderMap<T>();

    // Check if shader already created
    const auto &iterator = shader_map.find(shader_name);
    if (iterator != shader_map.cend())
    {
        return iterator->second;
    }

    // Create a shader object
    auto shader = std::make_shared<T>();

    shader->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    shader->set_name(shader_name.c_str());
    shader_map.insert(std::make_pair(shader_name, shader));

    // TODO: fix file name: R4 removes '(' and all symbols after
    shader->file_name   = { shader_name + ShaderTypeTraits<T>::file_extension };
    shader->entry_point = "main";
    shader->stage       = ShaderTypeTraits<T>::stage;

    bool result = CompileShader( shader_name
                               , shader.get()
    );
    R_ASSERT(result);

    return shader;
}
