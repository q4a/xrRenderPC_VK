#ifndef RESOURCES_SHADER_ELEMENT_H_
#define RESOURCES_SHADER_ELEMENT_H_

#include <array>
#include <memory>

#include "resources/pipeline_shader.h"
#include "resources/shader_pass.h"

enum class ShaderPassType
{
    Forward,
    Deferred,

    MaxNumber
};
constexpr auto max_shader_passes =
        static_cast<std::size_t>(ShaderPassType::MaxNumber);

struct ShaderElement
{
    bool IsEqual(const ShaderElement &element) const;

    svector<std::shared_ptr<ShaderPass>, max_shader_passes> shader_passes;
};

#endif // RESOURCES_SHADER_ELEMENT_H_
