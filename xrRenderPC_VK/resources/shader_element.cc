#include <algorithm>

#include "resources/manager.h"

#include "resources/shader_element.h"


/**
 *
 */
bool
ShaderElement::IsEqual
        ( const ShaderElement &element
        ) const
{
    if (element.shader_passes.size() != this->shader_passes.size())
    {
        return false;
    }

    for (const auto& pass : shader_passes)
    {
        // TODO
    }

    return true;
}


/**
 *
 */
std::shared_ptr<ShaderElement>
ResourceManager::CreateShaderElement
    ( const ShaderElement &element
    )
{
    if (element.shader_passes.empty())
    {
        return {};
    }

    /* Check if we already have the same element created */
    auto is_equal = [&](const auto &element_ptr) -> bool
    {
        return element.IsEqual(*element_ptr);
    };

    const auto &iterator = std::find_if( elements_.cbegin()
                                       , elements_.cend()
                                       , is_equal
    );

    if (iterator != elements_.cend())
    {
        return *iterator;
    }

    const auto ptr = std::make_shared<ShaderElement>(element);
    elements_.push_back(ptr);

    return ptr;
}
