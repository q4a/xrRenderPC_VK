#include "device/device.h"
#include "backend/backend.h"
#include "frontend/render.h"
#include "resources/blender_screen_set.h"

#include "resources/blender_compiler.h"

extern vk::PipelineColorBlendStateCreateInfo   state__color_blend;
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

    const auto &texture = resources.CreateTexture(texture_name);
    pass.textures[resource_name] = texture;
}


void
BlenderCompiler::PassSampler
        ( const std::string &resource_name
        )
{
    SamplerDescription description;

    if (resource_name == "smp_base")
    {
        if (hw.caps.device_features.samplerAnisotropy)
        {
            description.create_info.setAnisotropyEnable(true);
            description.create_info.maxAnisotropy = 1.0f; // TODO: should be console variable
        }
        else
        {
            Msg("! Anisotropic filtering isn't supported by HW");
        }
        description.create_info.setAddressModeU(vk::SamplerAddressMode::eMirroredRepeat);
        description.create_info.setAddressModeV(vk::SamplerAddressMode::eMirroredRepeat);
        description.create_info.setAddressModeW(vk::SamplerAddressMode::eMirroredRepeat);
    }
    else
    {
        R_ASSERT(false);
    }

    pass.samplers[resource_name] = description;
}


void
BlenderCompiler::SamplerAddressing
        ( const std::string      &sampler_name
        , vk::SamplerAddressMode  mode
        )
{
    const auto &iterator = pass.samplers.find(sampler_name);
    R_ASSERT2(iterator != pass.samplers.cend(), "Unknown sampler resource");

    auto &description = iterator->second;
    description.create_info
        .setAddressModeU(mode)
        .setAddressModeV(mode)
        .setAddressModeW(mode);
}


void
BlenderCompiler::PassAlphaBlendMode
(bool alpha_blend, vk::BlendFactor, vk::BlendFactor)
{

}

void
BlenderCompiler::PassAlphaBlendKey(bool, u32 alpha_reference)
{
    //clamp(alpha_reference, 0u, 0u);
}


void
BlenderCompiler::PassBlend
        ( bool            alpha_blend
        , vk::BlendFactor blend_source
        , vk::BlendFactor blend_destination
        , bool            alpha_test
        , u32             alpha_reference
        )
{
    PassAlphaBlendMode( alpha_blend
                      , blend_source
                      , blend_destination
    );
    PassAlphaBlendKey( alpha_test
                     , alpha_reference
    );
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

StreamBuffer ubo(512, BufferType::Uniform);

void
BlenderCompiler::SetMapping() // standard bindings
{
    // create ubo -- separate!
    ubo.Create();
    
    
    // update in backend set element
    void *ptr = (void *)ubo.cpu_buffer_->allocation_info.pMappedData;
    struct {
        float w, h, w1, h1;
    } abc;

    abc.w = hw.draw_rect.width;
    abc.h = hw.draw_rect.height;
    abc.w1 = 1.0f / hw.draw_rect.width;
    abc.h1 = 1.0f / hw.draw_rect.height;
    memcpy(ptr, &abc, sizeof(abc));

    ubo.position_ += sizeof(abc);
    ubo.Sync();
}

void
BlenderCompiler::PassEnd()
{
    // Create constant buffers
    // TODO: rework me! I know how!


    // Create samplers objects
    for (auto &[name, description] : pass.samplers)
    {
        description.sampler =
            hw.device->createSampler(description.create_info);
    }

    pass.CreatePipelineLayout();
    //pass.AllocateDescriptors();
    pass.CreatePipeline();

    SetMapping();

    /* Register shader pass resource and add it
     * into parent shader element
     */
    const auto pass_ptr = resources.CreateShaderPass(pass);
    shader_element->shader_passes.push_back(pass_ptr);
}
