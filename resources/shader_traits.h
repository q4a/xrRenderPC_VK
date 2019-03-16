#pragma once

#include "xrCore/xr_resource.h"

struct PipelineShader
    : public xr_resource_named
{
    vk::UniqueShaderModule spirv;
    // constant table
};

struct VertexShader
    : public PipelineShader
{

};

struct FragmentShader
    : public PipelineShader
{

};

template <class t>
struct ShaderTypeTraits;

using VertexShaders = std::map<std::string, std::shared_ptr<VertexShader>>;

template <>
struct ShaderTypeTraits<VertexShader>
{
    using MapType = VertexShaders;

    static inline std::string file_extension = ".vs";
};

//-----------------------------------------------------------------------------

using FragmentShaders = std::map<std::string, std::shared_ptr<FragmentShader>>;

template <>
struct ShaderTypeTraits<FragmentShader>
{
    using MapType = FragmentShaders;

    static inline std::string file_extension = ".ps";
};
