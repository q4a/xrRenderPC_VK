#include "resources/manager.h"
#include "resources/blender_compiler.h"

#include "resources/shader.h"

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
    std::shared_ptr<Shader> shader;

    // TODO: check for LUA shaders
    auto blender = GetBlender(shader_name);
    if (!blender)
    {
        return shader; // null
    }

    shader = std::make_shared<Shader>();

    // Initialize compiler
    BlenderCompiler compiler;
    ParseList(textures,  compiler.textures);
    ParseList(constants, compiler.constants);
    ParseList(matrices,  compiler.matrices);

    compiler.blender = blender;
    compiler.detail  = false; // TODO

    // Compile LOD elemets

    {
        compiler.current_element = ShaderElementType::NormalHq;

        ShaderElement element;
        compiler.Compile(element);

        const auto index = static_cast<std::size_t>(compiler.current_element);
        shader->elements[index] = CreateShaderElement(element);
    }

    // TODO: other passes

    // TODO: check if unique
    shaders_.push_back(shader);
    return shader;
}
