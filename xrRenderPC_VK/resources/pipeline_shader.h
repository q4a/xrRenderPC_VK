#ifndef RESOURCES_PIPELINE_SHADER_H_
#define RESOURCES_PIPELINE_SHADER_H_

#include <string>
#include <vector>

#include "xrCore/xr_resource.h"

enum class ShaderStage
{
    Vertex,
    Fragment
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
    // TODO: constant table
};

typedef struct
{
    vk::Format    format;
    std::uint32_t location;
    std::uint32_t offset;
} VertexInput;

struct VertexShader
    : public PipelineShader
{
    std::size_t stride_size = 0;
    std::vector<VertexInput> inputs;
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
