#include <shaderc/shaderc.hpp>

#include "device/device.h"
#include "frontend/render.h"

#include "resources/manager.h"

#pragma comment(lib, "shaderc_sharedd")

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
    std::string path{ frontend.getShaderPath() };
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

class Includer
    : public shaderc::CompileOptions::IncluderInterface
{
public:
    shaderc_include_result *GetInclude
            ( const char *requested_source
            , shaderc_include_type type
            , const char *requesting_source
            , size_t include_depth
            ) override
    {
        std::string path { frontend.getShaderPath() };
        path += requested_source;

        IReader *rstream = FS.r_open("$game_shaders$", path.c_str());
        if (!rstream)
        {
            // check outside of shaders directory
            rstream = FS.r_open("$game_shaders$", requested_source);
            if (!rstream)
            {
                return nullptr;
            }
        }

        auto result = new shaderc_include_result;
        result->source_name        = path.c_str();
        result->source_name_length = path.size();

        const std::size_t size = rstream->length();
        result->content_length = size;
        char *data = xr_alloc<char>(size + 1);
        CopyMemory(data, rstream->pointer(), size);
        data[size] = 0;
        result->content = data;

        FS.r_close(rstream);

        return result;
    }

    void ReleaseInclude
            ( shaderc_include_result *data
            ) override
    {
        if (data)
        {
            xr_delete<const char>(data->content);
            delete data;
        }
    }

    Includer() = default;
    ~Includer() = default;
};

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
    // TODO: create hash name

    // TODO: check for compiler cache

    // TODO: generate file name

    bool found_in_cache = false;

    rstream->seek(0);

    if (!found_in_cache)
    {
        std::vector<char> source_code(rstream->length());
        CopyMemory(source_code.data(), rstream->pointer(), source_code.size());
        source_code.push_back(0);

        auto kind = shaderc_shader_kind::shaderc_glsl_vertex_shader;

        shaderc::Compiler compiler{};
        shaderc::CompileOptions options{};

        options.SetSourceLanguage(shaderc_source_language_hlsl);
        options.SetIncluder(std::make_unique<Includer>());

        shaderc::CompilationResult output =
            compiler.CompileGlslToSpv( source_code.data()
                                     , kind
                                     , name.c_str()
                                     , entry_point.c_str()
                                     , options
            );

        if (output.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            Msg("! Shader compilation error");
            Msg("%s", output.GetErrorMessage());

            return false;
        }

        std::vector<std::int32_t> spirv{ output.cbegin(), output.cend() };
    }

    return true;
}
