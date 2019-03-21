#include "device/device.h"
#include "backend/backend.h"
#include "frontend/render.h"
#include "resources/blender_screen_set.h"

#include "resources/blender_compiler.h"

extern vk::PipelineColorBlendStateCreateInfo   state__color_blend;
extern vk::PipelineDepthStencilStateCreateInfo state__depth_stencil;
extern vk::PipelineColorBlendAttachmentState   color_blend_attachment;

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
    /* Set default pass params
     */
    PassZtest( true // Z test
             , true // Z write
    );
    PassBlend( false                    // Alpha blend
             , vk::BlendFactor::eOne    // source factor
             , vk::BlendFactor::eZero   // destination
             , false                    // Alpha test
             , 0                        // Alpha reference
    );
    PassLightFog( false // light
                , false // fog
    );

    /* Load and compile shaders
     */
    pass.vertex_shader   = resources.CreateVertexShader(vertex_shader);
    pass.fragment_shader = resources.CreateFragmentShader(fragment_shader);

    pass.MergeResources(pass.vertex_shader);
    pass.MergeResources(pass.fragment_shader);

    if (fragment_shader == "null")
    {
        // If no fragment shader specified
        // disable fragment output
        BlendColorWriteMask( false // R
                           , false // G
                           , false // B
                           , false // A
        );
    }
}


void
BlenderCompiler::PassTexture
        ( const std::string &resource_name
        , const std::string &texture_name
        )
{
    const auto &iterator = pass.resources.find(resource_name);
    VERIFY(iterator != pass.resources.cend());
    VERIFY(iterator->second->type == vk::DescriptorType::eSampledImage);

    const auto &texture = resources.CreateTexture(texture_name);
    pass.textures[resource_name] = texture;
}


void
BlenderCompiler::PassSampler
        ( const std::string &resource_name
        )
{
    const auto &iterator = pass.resources.find(resource_name);
    VERIFY(iterator != pass.resources.cend());
    VERIFY(iterator->second->type == vk::DescriptorType::eSampler);

    // Add sampler to pass
    SamplerDescription description;
    pass.samplers[resource_name] = description;

    if (resource_name == "smp_base")
    {
        SamplerAnisotropy(resource_name, true);
        SamplerAddressing(resource_name, vk::SamplerAddressMode::eMirroredRepeat);
        return;
    }

    // TODO: handle all samplers
    R_ASSERT(false);
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
BlenderCompiler::SamplerAnisotropy
        ( const std::string &sampler_name
        , bool               enable
        )
{
    const auto &iterator = pass.samplers.find(sampler_name);
    R_ASSERT2(iterator != pass.samplers.cend(), "Unknown sampler resource");

    if (enable && !hw.caps.device_features.samplerAnisotropy)
    {
        Msg("! Anisotropic filtering isn't supported by HW");
        enable = false;
    }

    auto &description = iterator->second;
    description.create_info.setAnisotropyEnable(enable);
    description.create_info.maxAnisotropy = 1.0f; // TODO: should be console variable
}


void
BlenderCompiler::BlendAlphaMode
        ( bool alpha_blend
        , vk::BlendFactor
        , vk::BlendFactor
        )
{
    // TBI
}

void
BlenderCompiler::BlendAlphaReference
        ( bool          alpha_test
        , std::uint32_t alpha_reference
        )
{
    // TBI
}


void
BlenderCompiler::BlendColorWriteMask
        ( bool out_r
        , bool out_g
        , bool out_b
        , bool out_a
        )
{
    vk::ColorComponentFlags mask;
    if (out_r)
    {
        mask |= vk::ColorComponentFlagBits::eR;
    }
    if (out_g)
    {
        mask |= vk::ColorComponentFlagBits::eG;
    }
    if (out_b)
    {
        mask |= vk::ColorComponentFlagBits::eB;
    }
    if (out_a)
    {
        mask |= vk::ColorComponentFlagBits::eA;
    }
    color_blend_attachment.colorWriteMask = mask;
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
    BlendAlphaMode( alpha_blend
                  , blend_source
                  , blend_destination
    );
    BlendAlphaReference( alpha_test
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


void
BlenderCompiler::PassEnd()
{
    // Allocate constant buffers
    for (auto &[name, constant] : pass.constants)
    {
        /* If multiple swapchain images used we can't bind
         * the same UBO for all the frames: it may cause
         * a racing between host-out DMA transfer on update
         * and shader fetch. So we allocate one buffer object
         * per frame.
         */
        for (auto index = 0; index < hw.baseRt.size(); index++)
        {
            auto buffer_pointer =
                std::shared_ptr<StreamBuffer>{ new StreamBuffer{ constant->size
                                                               , BufferType::Uniform
                }};

            buffer_pointer->Create();
            constant->buffers.push_back(buffer_pointer);
        }

        SetDefaultBindings(*constant);
    }

    // Create samplers objects
    for (auto &[name, description] : pass.samplers)
    {
        description.sampler =
            hw.device->createSampler(description.create_info);
    }

    pass.CreatePipelineLayout();
    pass.AllocateDescriptors();
    pass.CreatePipeline();

    /* Register shader pass resource and add it
     * into parent shader element
     */
    const auto pass_resource = resources.CreateShaderPass(pass);
    shader_element->shader_passes.push_back(pass_resource);
}
