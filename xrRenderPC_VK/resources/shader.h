#ifndef RESOURCES_SHADER_H_
#define RESOURCES_SHADER_H_

#include <array>
#include <memory>

#include "resources/element.h"

constexpr std::size_t max_elements = 6;

class Shader
{
public:
    std::array<std::shared_ptr<Element>, max_elements> elements;
};

#endif // RESOURCES_SHADER_H_
