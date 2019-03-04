#include "frontend/render.h"
#include "resources/blender_screen_set.h"

#include "resources/blender_compiler.h"

void
BlenderCompiler::Compile
        ( ShaderElement &element
        )
{
    // TBI
    blender->Compile(*this);
}

void
BlenderCompiler::PassBegin
        ( const std::string &vertex_shader
        , const std::string &fragment_shader
        )
{
    /* Load and compile shaders
     */
    pass.vertex_shader =
        frontend.resources_->CreateVertexShader(vertex_shader);
    pass.fragment_shader =
        frontend.resources_->CreateFragmentShader(fragment_shader);

    /* Create shader stages
     */
    {
        const auto index = static_cast<std::size_t>(ShaderStage::Vertex);

        auto &vertex = shader_stages_[index];
        vertex
            .setModule(pass.vertex_shader->module.get())
            .setPName(pass.vertex_shader->file_name.c_str())
            .setStage(vk::ShaderStageFlagBits::eVertex);

        // TODO fill in vertex input state
    }

    {
        const auto index = static_cast<std::size_t>(ShaderStage::Fragment);

        auto &vertex = shader_stages_[index];
        vertex
            .setModule(pass.fragment_shader->module.get())
            .setPName(pass.fragment_shader->file_name.c_str())
            .setStage(vk::ShaderStageFlagBits::eFragment);
    }

    // TODO: Merge constant tables
}


void
BlenderCompiler::PassTexture
        ( const std::string &name
        , const std::string &texture
        )
{

}


void
BlenderCompiler::PassEnd()
{
    /* Create graphics pipeline
     */
    vk::PipelineInputAssemblyStateCreateInfo()
        .setTopology(vk::PrimitiveTopology::eTriangleList); // FIXME: this should be defined by renderer


    /* Register shader pass resource and add it
     * into parent shader element
     */
    const auto pass_ptr = frontend.resources_->CreateShaderPass(pass);
    shader_element->shader_passes.push_back(pass_ptr);
}
