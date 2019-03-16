#include "device/device.h"
#include "backend/backend.h"
#include "frontend/render.h"
#include "resources/blender_screen_set.h"

#include "resources/blender_compiler.h"

extern vk::PipelineDepthStencilStateCreateInfo state__depth_stencil;

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
    pass.vertex_shader   = resources.CreateVertexShader(vertex_shader);
    pass.fragment_shader = resources.CreateFragmentShader(fragment_shader);

    pass.MergeConstants(pass.vertex_shader->constants);
    pass.MergeConstants(pass.fragment_shader->constants);
}


void
BlenderCompiler::PassTexture
        ( const std::string &resource_name
        , const std::string &texture_name
        )
{
    const auto &iterator = pass.resources.find(resource_name);
    VERIFY(iterator != pass.resources.cend());
    VERIFY(iterator->second.type == vk::DescriptorType::eSampledImage);
}


void
BlenderCompiler::PassZtest
        ( bool z_test
        , bool z_write
        , bool invert_test /* = false */
        )
{
    state__depth_stencil.setDepthTestEnable(z_test);
    state__depth_stencil.setDepthWriteEnable(z_write);
    if (invert_test)
    {
        state__depth_stencil.setDepthCompareOp(vk::CompareOp::eGreater);
    }
    else
    {
        state__depth_stencil.setDepthCompareOp(vk::CompareOp::eLess);
    }
}


void
BlenderCompiler::PassEnd()
{
    pass.CreatePipelineLayout();
    pass.AllocateDescriptors();
    pass.CreatePipeline();

    /* Register shader pass resource and add it
     * into parent shader element
     */
    const auto pass_ptr = resources.CreateShaderPass(pass);
    shader_element->shader_passes.push_back(pass_ptr);
}
