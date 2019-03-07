#include <algorithm>
#include <iomanip>
#include <sstream>

#include <shaderc/shaderc.hpp>

#include "xrCore/FileCRC32.h"

#include "spirv_cross/spirv_cross.hpp"
#include "spirv_cross/spirv_parser.hpp"
#include "spirv_cross/spirv_hlsl.hpp"

#include "device/device.h"
#include "frontend/render.h"
#include "resources/manager.h"

#include "resources/pipeline_shader.h"


/*!
 * \brief   Converts SPIR-V type into Vulkan's `VkFormat`
 *
 * The easiest way is to use `type_to_glsl()` from spirv-cross compiler,
 * but unfortunately no implementation exposes it (the method is private).
 * This function is very simple and only basic types handled.
 */
static vk::Format
GetTypeFormat
        ( const spirv_cross::SPIRType &type
        )
{
    R_ASSERT(type.width == 32);

    vk::Format format = vk::Format::eUndefined;

    // TODO: Only vectors matter by now
    if (type.vecsize > 1 && type.columns == 1)
    {
        switch (type.basetype)
        {
        case spirv_cross::SPIRType::Float:
            switch (type.vecsize)
            {
            case 2:
                format = vk::Format::eR32G32Sfloat;
                break;
            case 3:
                format = vk::Format::eR32G32B32Sfloat;
                break;
            case 4:
                format = vk::Format::eR32G32B32A32Sfloat;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        return format;
    }

    R_ASSERT(format != vk::Format::eUndefined);
    return format;
}


/*!
 * \brief   Calculates the size of a given SPIR-V type
 */
static std::size_t
GetTypeSize
        ( const spirv_cross::SPIRType &type
        )
{
    const auto element_size       = type.width / 8;
    const auto elements_in_row    = type.vecsize;
    const auto elements_in_column = type.columns;

    return element_size * elements_in_row * elements_in_column;
}


/*!
 * \brief   Creates Vulkan shader module from assigned SPIR-V IR.
 *
 * This module will be attached to the graphics pipeline at
 * the end of blender compiler pass.
 */
void
PipelineShader::CreateModule()
{
    R_ASSERT2(spirv.size(), "Can't create from empty SPIR-V data");

    const auto size_in_bytes = spirv.size() * sizeof(spirv[0]);
    const auto module_create_info =vk::ShaderModuleCreateInfo()
        .setPCode(spirv.data())
        .setCodeSize(size_in_bytes);
    module = hw.device->createShaderModuleUnique(module_create_info);
}


/*!
 * \brief   Parses shader resources and creates module's constant table.
 *
 * The function uses `spirv-cross` compiler for SPIR-V shader reflection.
 * Then iterates over resources to create a constant table, sampler or
 * vertex format representation.
 */
void
PipelineShader::ParseResources()
{
    spirv_cross::Parser parser{ spirv.data(), spirv.size() };
    parser.parse();

    //spirv_cross::CompilerHLSL compiler(std::move(parser.get_parsed_ir()));
    spirv_cross::CompilerHLSL compiler(std::move(parser.get_parsed_ir()));

    // Check if specified entry point is valid
    // NOTE: in case of DX the entry point is `main` for ages (proof?)
    const auto &entry_points = compiler.get_entry_points();
    const auto &iterator = std::find( entry_points.cbegin()
                                    , entry_points.cend()
                                    , entry_point
    );
    R_ASSERT2(iterator != entry_points.cend(), "Wrong entry point specified!");

    // TODO: Check for pipeline stage binding point

    const auto &resources = compiler.get_shader_resources();

    // In case of vertex shader we need to additionaly create a vertex
    // format descriptor. It will used for vertex input stage description.
    if (stage == ShaderStage::Vertex)
    {
        VertexShader *shader_ptr = static_cast<VertexShader *>(this);

        std::uint32_t offset = 0;

        const auto &inputs = resources.stage_inputs;
        for (const auto &input : inputs)
        {
            VertexInput vertex_input;
            vertex_input.offset   = offset;
            vertex_input.location =
                compiler.get_decoration(input.id, spv::DecorationLocation);

            const auto &type = compiler.get_type(input.type_id);
            vertex_input.format = GetTypeFormat(type);

            const auto size = GetTypeSize(type);
            shader_ptr->stride_size += size;
            offset += size;

            shader_ptr->inputs.push_back(vertex_input);
        }
    }
}


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


/**
 *
 */
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

        /* TODO: It's pity I can't use any container here
         *       since its type will be erased by passing
         *       source data by pointer: a nice riddle to
         *       think about.
         */
        const std::size_t size = rstream->length();
        result->content_length = size;
        char *data = xr_alloc<char>(size);
        CopyMemory( data
                  , rstream->pointer()
                  , size
        );
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
 * TODO: This function looks pretty heavy and
 *       should be decomposed
 */
bool
ResourceManager::CompileShader
        ( const std::string &name
        , PipelineShader *shader
        )
{
    std::map<std::string, std::string> defines = shader_options_;
    std::stringstream shader_hash_name;

    /* Form a shader cache name based on render options set
     */

    {
        const auto smap_size = frontend.o.shadow_map_size;
        shader_hash_name << std::setfill('0')
                         << std::setw(4)
                         << smap_size;
        defines["SMAP_size"] = std::to_string(smap_size);
    }

    if (true)
    {
        // TODO: and so on and so forth
    }

    /* Generate names for compiler's artefacts and cache directory
     * The compilation cache stored in tree like:
     *
     * <$app_data_root>
     *    |
     *    `- shaders_cache\
     *          |
     *          ` vk\
     *              |
     *              `- shader_file_name.ext\
     *                  |
     *                  +  shader_file_name.ext
     *                  +  <render options hash>.asm
     *                  `- <render options hash>.spirv
     */

    string_path cache_folder;
    strconcat( sizeof(cache_folder)
             , cache_folder
             , "vk" DELIMITER
             , shader->file_name.c_str()
    );

    string_path cache_relative_path;
    strconcat( sizeof(cache_relative_path)
             , cache_relative_path
             , "shaders_cache" DELIMITER
             , cache_folder
             , DELIMITER
             , shader_hash_name.str().c_str()
             , ".spirv"
    );

    string_path cache_absolute_path;
    FS.update_path(cache_absolute_path
                  , "$app_data_root$"
                  , cache_relative_path
    );

    /* Open shader source and create reader stream
     */
    string_path shader_source_folder;
    FS.update_path( shader_source_folder
                  , "$game_shaders$"
                  , frontend.getShaderPath()
    );

    string_path shader_source_path;
    strconcat( sizeof(shader_source_path)
             , shader_source_path
             , shader_source_folder
             , shader->file_name.c_str()
    );

    IReader *rsource = FS.r_open(shader_source_path);
    R_ASSERT2(rsource, "Unable to find shader source");

    /* Calculate shader's CRC
     */
    u32 shader_crc = 0;
    getFileCrc32( rsource
                , shader_source_folder
                , shader_crc
    );

    rsource->rewind();

    /* Copy shader source to feed it into further stages
     */
    std::vector<char> shader_source(rsource->length());
    CopyMemory( shader_source.data()
              , rsource->pointer()
              , rsource->length()
    );

    FS.r_close(rsource); // No need the stream anymore

    /* Dump shader text source
     */
    if (frontend.o.shader__dump_source)
    {
        string_path source_path;
        strconcat( sizeof(source_path)
                 , source_path
                 , "shaders_cache" DELIMITER
                 , cache_folder
                 , DELIMITER
                 , shader->file_name.c_str()
        );

        IWriter *wstream = FS.w_open_ex("$app_data_root$", source_path);
        if (wstream)
        {
            wstream->w(shader_source.data(), shader_source.size());
            FS.w_close(wstream);
        }
    }

    bool is_cache_valid = false;

    if (FS.exist(cache_absolute_path))
    {
        /* Load cached SPIR-V
         */

        // TODO: check for CRC matching

        IReader *rcache = FS.r_open(cache_absolute_path);
        R_ASSERT(rcache);

        auto &spirv = shader->spirv;
        const auto instructions_num = rcache->length() / sizeof(std::uint32_t);
        spirv.resize(instructions_num);
        CopyMemory( spirv.data()
                  , rcache->pointer()
                  , rcache->length()
        );

        is_cache_valid = true;

        FS.r_close(rcache);
    }

    if (!is_cache_valid)
    {
        /* Compile shader from scratch
         */

        shaderc_shader_kind shader_kind;
        switch (shader->stage)
        {
        case ShaderStage::Vertex:
            shader_kind = shaderc_shader_kind::shaderc_glsl_vertex_shader;
            break;
        case ShaderStage::Fragment:
            shader_kind = shaderc_shader_kind::shaderc_glsl_fragment_shader;
            break;
        default:
            R_ASSERT2(false, "Unexpected shader stage passed");
            break;
        }

        shaderc::Compiler compiler{};
        shaderc::CompileOptions options{};

        options.SetSourceLanguage(shaderc_source_language_hlsl);
        options.SetIncluder(std::make_unique<Includer>());
        options.SetHlslIoMapping(true);

        shaderc::CompilationResult output =
            compiler.CompileGlslToSpv( shader_source.data()
                                     , shader_kind
                                     , name.c_str()
                                     , shader->entry_point.c_str()
                                     , options
            );

        if (output.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            Msg("! Shader compilation error");
            Msg("%s", output.GetErrorMessage());

            return false;
        }

        shader->spirv = { output.cbegin(), output.cend() };

        /* Write SPIRV into cache
         */
        IWriter *wcache = FS.w_open(cache_absolute_path);
        R_ASSERT2(wcache, "Unable to create cache");

        /* TODO: To put the CRC. I don't think that following
         *       original code's approach is good: sometimes
         *       it's convinient to work with SPIR-V blobs
         *       using various 3rd party software. In this
         *       case the CRC in the header will broke such
         *       compatibility
         */

        const auto &spirv = shader->spirv;
        const auto size_in_bytes = spirv.size() * sizeof(spirv[0]);
        wcache->w(spirv.data(), size_in_bytes);
        FS.w_close(wcache);
    }

    /* Disassemble if needed
     */
    if (frontend.o.shader__disassemble)
    {
        // TBI
    }

    shader->ParseResources();

    shader->CreateModule();

    return true;
}
