#include "resources/manager.h"

#include "resources/shader_pass.h"


/**
 *
 */
bool
ShaderPass::IsEqual
        ( const ShaderPass &pass
        ) const
{
    return false; // FIXME: of course not!
}


/**
 *
 */
std::shared_ptr<ShaderPass>
ResourceManager::CreateShaderPass
        ( const ShaderPass &pass
        )
{
    /* Check if we already have the same pass created */
    auto is_equal = [&](const auto &pass_ptr) -> bool
    {
        return pass.IsEqual(*pass_ptr);
    };

    const auto &iterator = std::find_if( shader_passes_.cbegin()
                                       , shader_passes_.cend()
                                       , is_equal
    );

    if (iterator == shader_passes_.cend())
    {
        return *iterator;
    }

    const auto ptr = std::make_shared<ShaderPass>(pass);
    shader_passes_.push_back(ptr);

    return ptr;
}
