#include "device/device.h"
#include "frontend/render.h"
#include "resources/blender_screen_set.h"

#include "resources/blender_compiler.h"

void
BlenderCompiler::Compile
        ( ShaderElement &element
        )
{
    /* Store pointer to target element for further
     * updates while going through blender passes
     */
    shader_element = &element;

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
    CreatePipeline();

    /* Register shader pass resource and add it
     * into parent shader element
     */
    const auto pass_ptr = frontend.resources_->CreateShaderPass(pass);
    shader_element->shader_passes.push_back(pass_ptr);
}


/*!
 * \brief   Create GPO for current render pass
 *
 * TODO: probably most of the structures can be static stencils to speedup
 *       the pipeline creation.
 */
void
BlenderCompiler::CreatePipeline()
{
    /* Shader stages
     */
    std::array< vk::PipelineShaderStageCreateInfo, max_shader_stages> shader_stages;

    {
        const auto index = static_cast<std::size_t>(ShaderStage::Vertex);

        auto &vertex = shader_stages[index];
        vertex
            .setModule(pass.vertex_shader->module.get())
            .setPName(pass.vertex_shader->file_name.c_str())
            .setStage(vk::ShaderStageFlagBits::eVertex);
    }

    {
        const auto index = static_cast<std::size_t>(ShaderStage::Fragment);

        auto &vertex = shader_stages[index];
        vertex
            .setModule(pass.fragment_shader->module.get())
            .setPName(pass.fragment_shader->file_name.c_str())
            .setStage(vk::ShaderStageFlagBits::eFragment);
    }

    /* Vertex inputs
     */
    const auto input_binding_description = vk::VertexInputBindingDescription()
        .setBinding(0)
        .setStride(pass.vertex_shader->stride_size)
        .setInputRate(vk::VertexInputRate::eVertex);

    const auto input_state_create_info = vk::PipelineVertexInputStateCreateInfo()
        .setVertexBindingDescriptionCount(1)
        .setPVertexBindingDescriptions(&input_binding_description)
        .setVertexAttributeDescriptionCount(pass.vertex_shader->inputs.size())
        .setPVertexAttributeDescriptions(pass.vertex_shader->inputs.data());

    /* Input assembly
     */
    vk::PipelineInputAssemblyStateCreateInfo()
        .setTopology(vk::PrimitiveTopology::eTriangleList); // FIXME: this should be defined by renderer

    /* Viewport and scissors
     */
    const auto viewport = vk::Viewport()
        .setWidth(hw.draw_rect.width)
        .setHeight(hw.draw_rect.height);
    // TODO: max depth!

    const auto scissor = vk::Rect2D()
        .setExtent(hw.draw_rect);

    const auto viewport_create_info = vk::PipelineViewportStateCreateInfo()
        .setViewportCount(1)
        .setPViewports(&viewport)
        .setScissorCount(1)
        .setPScissors(&scissor);

    // TBI::..
}
