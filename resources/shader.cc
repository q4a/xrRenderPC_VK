#include "resources/manager.h"
#include "resources/blender_compiler.h"

#include "resources/shader.h"


/*!
 * \brief   Compares two shader objects
 * \param [in] reference shader to compare with
 * \return result
 */
bool
Shader::IsEqual
        ( const Shader &reference
        ) const
{
    return false;
}



/**
 *
 */
std::shared_ptr<Shader>
ResourceManager::CreateShader
        ( const std::string &shader_name
        , const std::string &textures
        , const std::string &constants
        , const std::string &matrices
        )
{
    std::shared_ptr<Shader> result{};

    if (ScriptingHasShader(shader_name))
    {
        result = CreateShaderLua( shader_name
                                , textures
        );
    }
    else
    {
        result = CreateShaderCpp( shader_name
                                , textures
                                , constants
                                , matrices
        );

        // TODO: implement fallback variant
    }

    return result;
}


void
ResourceManager::DestroyShader
        ( std::shared_ptr<Shader> &shader
        )
{
    if (shader)
    {
        // TODO
    }
}


/**
 *
 */
std::shared_ptr<Shader>
ResourceManager::CreateShaderCpp
        ( const std::string &shader_name
        , const std::string &textures
        , const std::string &constants
        , const std::string &matrices
        )
{
    std::shared_ptr<Shader> shader_resource;

    auto blender = GetBlender(shader_name);
    VERIFY(blender);

    shader_resource = std::make_shared<Shader>();
    shader_resource->set_name(shader_name.c_str());

    // Initialize compiler
    BlenderCompiler compiler;
    ParseList(textures,  compiler.textures);
    RemoveTexturesExtension(compiler.textures);
    ParseList(constants, compiler.constants);
    ParseList(matrices,  compiler.matrices);

    compiler.blender = blender;

    /* Compile LOD elemets
     */

    { // LOD0 - HQ
        compiler.current_element = ShaderElementType::NormalHq;
        compiler.detail =
            texture_description_.GetDetailTexture( compiler.textures[0]
                                                 , compiler.detail_texture
                                                 , compiler.detail_scaler
            );
        shader_resource->elements[0] = compiler.Compile();
    }

    { // LOD1
        compiler.current_element = ShaderElementType::NormalLq;
        compiler.detail =
            texture_description_.GetDetailTexture( compiler.textures[0]
                                                 , compiler.detail_texture
                                                 , compiler.detail_scaler
            );
        shader_resource->elements[1] = compiler.Compile();
    }

    { // LOD2
        compiler.current_element = ShaderElementType::LightPoint;
        compiler.detail =  // TODO: not sure if can be detailed
            texture_description_.GetDetailTexture(compiler.textures[0]
                                                 , compiler.detail_texture
                                                 , compiler.detail_scaler
            );
        shader_resource->elements[2] = compiler.Compile();
    }

    { // LOD3
        compiler.current_element = ShaderElementType::LightSpot;
        compiler.detail = // TODO: not sure if can be detailed
            texture_description_.GetDetailTexture(compiler.textures[0]
                , compiler.detail_texture
                , compiler.detail_scaler
            );
        shader_resource->elements[3] = compiler.Compile();
    }

    { // LOD4
        compiler.current_element = ShaderElementType::LightSpecial;
        compiler.detail = true; // TODO: what the hack? :)
        shader_resource->elements[4] = compiler.Compile();
    }

    { // LOD5 -- ?
        compiler.current_element = ShaderElementType::Unknown;
        compiler.detail = false;
        shader_resource->elements[5] = compiler.Compile();
    }

    // Check if we already have this shader created
    const auto predicate = [&](const auto &shader) -> bool
    {
        return shader_resource->IsEqual(*shader);
    };
    const auto &iterator =
        std::find_if( shaders_.cbegin()
                    , shaders_.cend()
                    , predicate
        );

    if (iterator != shaders_.cend())
    {
        return *iterator;
    }

    shaders_.push_back(shader_resource);
    return shader_resource;
}
