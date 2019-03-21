#ifndef RESOURCES_SHADER_H_
#define RESOURCES_SHADER_H_

#include <array>
#include <memory>

#include "xrCore/xr_resource.h"

#include "resources/shader_element.h"

constexpr std::size_t max_elements = 6;

class Shader
    : public xr_resource_named
{
public:
    std::array<std::shared_ptr<ShaderElement>, max_elements> elements;
};

#endif // RESOURCES_SHADER_H_
