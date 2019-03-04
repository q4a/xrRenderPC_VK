#ifndef RESOURCES_PIPELINE_SHADER_H_
#define RESOURCES_PIPELINE_SHADER_H_

#include <vector>

#include "xrCore/xr_resource.h"

enum class ShaderStage
{
    Vertex,
    Fragment
};

struct PipelineShader
    : public xr_resource_named
{
    std::string file_name;
    std::string entry_point;
    ShaderStage stage;

    std::vector<std::uint32_t> spirv;
    vk::UniqueShaderModule module;
    // TODO: constant table
};

struct VertexShader
    : public PipelineShader
{
    // TODO: vertex input format
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
