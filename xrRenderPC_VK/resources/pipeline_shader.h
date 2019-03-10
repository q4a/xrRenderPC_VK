#ifndef RESOURCES_PIPELINE_SHADER_H_
#define RESOURCES_PIPELINE_SHADER_H_

#include <map>
#include <string>
#include <vector>

#include "xrCore/xr_resource.h"

#include "resources/constant_table.h"

enum class ShaderStage
{
    Vertex,
    Fragment
};

struct ImageResource
{
    std::uint32_t set;
    std::uint32_t binding;
};

class PipelineShader
    : public xr_resource_named
{
    friend class ResourceManager;

    void CreateModule();
    void ParseResources();
public:
    std::string file_name;
    std::string entry_point;
    ShaderStage stage;

    std::vector<std::uint32_t> spirv;
    vk::UniqueShaderModule module;
    ConstantTable constant_table;
    std::map<std::string, ImageResource> samplers;
    std::map<std::string, ImageResource> textures;
};

struct VertexShader
    : public PipelineShader
{
    std::size_t stride_size = 0;
    std::vector<vk::VertexInputAttributeDescription> inputs;
};

struct FragmentShader
    : public PipelineShader
{
    // Nothing special
};

template <typename T>
struct ShaderTypeTraits;


template <>
struct ShaderTypeTraits<VertexShader>
{
    static const inline std::string file_extension = ".vs";
    static const ShaderStage stage = ShaderStage::Vertex;
};


template <>
struct ShaderTypeTraits<FragmentShader>
{
    static const inline std::string file_extension = ".ps";
    static const ShaderStage stage = ShaderStage::Fragment;
};

#endif // RESOURCES_PIPELINE_SHADER_H_
