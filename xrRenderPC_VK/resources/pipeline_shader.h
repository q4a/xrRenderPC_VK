#ifndef RESOURCES_PIPELINE_SHADER_H_
#define RESOURCES_PIPELINE_SHADER_H_

#include <map>
#include <string>
#include <vector>

#include "xrCore/xr_resource.h"

#include "resources/constant_table.h"


class PipelineShader
    : public xr_resource_named
{
    friend class ResourceManager;

    void CreateModule();
    void ParseResources();
public:
    std::string file_name;
    std::string entry_point;
    vk::ShaderStageFlagBits stage;

    std::vector<std::uint32_t> spirv;
    vk::UniqueShaderModule module;
    std::map<std::string, ShaderResource> constants;
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
    static const vk::ShaderStageFlagBits stage =
        vk::ShaderStageFlagBits::eVertex;
};


template <>
struct ShaderTypeTraits<FragmentShader>
{
    static const inline std::string file_extension = ".ps";
    static const vk::ShaderStageFlagBits stage =
        vk::ShaderStageFlagBits::eFragment;
};

#endif // RESOURCES_PIPELINE_SHADER_H_
