#ifndef RESOURCES_SHADER_PASS_H_
#define RESOURCES_SHADER_PASS_H_

#include "resources/pipeline_shader.h"

struct ShaderPass
{
    bool IsEqual(const ShaderPass &pass) const;

    std::shared_ptr<VertexShader>   vertex_shader;
    std::shared_ptr<FragmentShader> fragment_shader;
};

#endif // RESOURCES_SHADER_PASS_H_
