#include "backend/backend.h"
#include "device/device.h"
#include "resources/manager.h"
#include "frontend/render.h"

#include "resources/shader_pass.h"


/*!
 * Pipeline states stencils
 */
static vk::PipelineVertexInputStateCreateInfo state__vertex_input =
{};
static vk::PipelineInputAssemblyStateCreateInfo state__input_assembly =
{
    {},
    vk::PrimitiveTopology::eTriangleList, // FIXME: this should be defined by renderer somehow
    0
};
static vk::PipelineTessellationStateCreateInfo state__tesselation =
{};
static vk::PipelineViewportStateCreateInfo state__viewport =
{};
static vk::PipelineRasterizationStateCreateInfo state__rasterization =
{};
static vk::PipelineMultisampleStateCreateInfo state__multisampling =
{};

static constexpr vk::DynamicState dynamic_states[] =
{
    vk::DynamicState::eScissor
};

static vk::PipelineDynamicStateCreateInfo state__dynamic =
{
    {},
    _countof(dynamic_states),
    dynamic_states
};

/*!
 * Pipeline shared stencils
 */
vk::PipelineColorBlendStateCreateInfo state__color_blend =
{};
vk::PipelineDepthStencilStateCreateInfo state__depth_stencil =
{};

vk::PipelineColorBlendAttachmentState color_blend_attachment =
{};

static vk::GraphicsPipelineCreateInfo pipeline_create_info =
{
    {}, // flags TODO: Allow derivatives
    0, // shader stages count
    nullptr, // shader stages pointer
    &state__vertex_input,
    &state__input_assembly,
    &state__tesselation,
    &state__viewport,
    &state__rasterization,
    &state__multisampling,
    &state__depth_stencil,
    &state__color_blend,
    &state__dynamic,
};


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

    if (iterator != shader_passes_.cend())
    {
        return *iterator;
    }

    const auto ptr = std::make_shared<ShaderPass>(pass);
    shader_passes_.push_back(ptr);

    return ptr;
}


void
ShaderPass::MergeResources
        ( const std::shared_ptr<PipelineShader> &shader
        )
{
    /* Merge constants
     */
    for (const auto &[name, constant] : shader->constants)
    {
        const auto &iterator = constants.find(name);
        if (iterator == constants.cend())
        {
            resources.insert(std::make_pair(name, constant));
        }
        else
        {
            // TODO: only access to the same UBOs from several stages allowed
            VERIFY(constant->IsEqual(*iterator->second));
            iterator->second->stage |= constant->stage; // update binding point
        }
        constants.insert(std::make_pair(name, constant));
    }

    /* Merge samplers
     */
    for (const auto &[name, sampler] : shader->samplers)
    {
        const auto &iterator = samplers.find(name);
        VERIFY(iterator == samplers.cend()); // only unique are allowed

        resources.insert(std::make_pair(name, sampler));
    }

    /* Merge textures
     */
    for (const auto &[name, texture] : shader->textures)
    {
        const auto &iterator = textures.find(name);
        VERIFY(iterator == textures.cend()); // only unique are allowed

        resources.insert(std::make_pair(name, texture));
    }
}


/*!
 * \brief   Creates pipeline layout to bind shaders resources
 */
void
ShaderPass::CreatePipelineLayout()
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings;

    for (const auto &resource : resources)
    {
        auto binding = vk::DescriptorSetLayoutBinding()
            .setBinding(resource.second->binding)
            .setDescriptorType(resource.second->type)
            .setDescriptorCount(1)
            .setStageFlags(resource.second->stage);

        bindings.push_back(binding);
    }

    const auto descriptors_layout_create_info =
        vk::DescriptorSetLayoutCreateInfo()
            .setPBindings(bindings.data())
            .setBindingCount(bindings.size());
    descriptors_layout =
        hw.device->createDescriptorSetLayout(descriptors_layout_create_info);


    const auto layout_create_info = vk::PipelineLayoutCreateInfo()
        .setPSetLayouts(&descriptors_layout)
        .setSetLayoutCount(1);

    pipeline_layout =
        hw.device->createPipelineLayout(layout_create_info);
}

void
ShaderPass::AllocateDescriptors()
{
    std::vector<vk::DescriptorPoolSize> pool_sizes;
    pool_sizes.resize(resources.size());

    const auto descriptors_count = hw.swapchain_images.size();

    auto i = 0;
    for (const auto &resource : resources)
    {
        pool_sizes[i].type = resource.second->type;
        pool_sizes[i].descriptorCount = descriptors_count;
        i++;
    }

    const auto pool_create_info = vk::DescriptorPoolCreateInfo()
        .setMaxSets(descriptors_count)
        .setPPoolSizes(pool_sizes.data())
        .setPoolSizeCount(pool_sizes.size());
    auto descriptor_pool = hw.device->createDescriptorPool(pool_create_info);

    std::vector<vk::DescriptorSetLayout> layouts(descriptors_count, descriptors_layout); // for const cast
    const auto descriptor_set_allocate_info = vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(descriptor_pool)
        .setDescriptorSetCount(layouts.size())
        .setPSetLayouts(layouts.data());
    descriptors =
        hw.device->allocateDescriptorSets(descriptor_set_allocate_info);
}


/*!
 * \brief   Create GPO for current render pass
 *
 * TODO: probably most of the structures can be static stencils to speedup
 *       the pipeline creation.
 */
void
ShaderPass::CreatePipeline()
{
    /* Shader stages
     */
    std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;

    if (vertex_shader)
    {
        const auto &vertex = vk::PipelineShaderStageCreateInfo()
            .setModule(vertex_shader->module.get())
            .setPName(vertex_shader->entry_point.c_str())
            .setStage(vertex_shader->stage);

        // TODO: since we fully rely on shader's data
        //       no need to treat each kind separately
        shader_stages.push_back(vertex);
    }

    if (fragment_shader)
    {
        const auto &fragment = vk::PipelineShaderStageCreateInfo()
            .setModule(fragment_shader->module.get())
            .setPName(fragment_shader->entry_point.c_str())
            .setStage(fragment_shader->stage);

        shader_stages.push_back(fragment);
    }

    /* Update states
     */

    // Vertex inputs
    const auto input_binding_description = vk::VertexInputBindingDescription()
        .setBinding(0)
        .setStride(vertex_shader->stride_size)
        .setInputRate(vk::VertexInputRate::eVertex);

    state__vertex_input
        .setVertexBindingDescriptionCount(1)
        .setPVertexBindingDescriptions(&input_binding_description)
        .setVertexAttributeDescriptionCount(vertex_shader->inputs.size())
        .setPVertexAttributeDescriptions(vertex_shader->inputs.data());

    // Viewport
    const auto viewport = vk::Viewport()
        .setWidth(hw.draw_rect.width)
        .setHeight(hw.draw_rect.height)
        .setMaxDepth(1.0f);

    const auto scissor = vk::Rect2D()
        .setExtent(hw.draw_rect);

    state__viewport
        .setViewportCount(1)
        .setPViewports(&viewport)
        .setScissorCount(1)
        .setPScissors(&scissor);

    //***
    state__rasterization.depthClampEnable = false;
    state__rasterization.rasterizerDiscardEnable = false;
    state__rasterization.lineWidth = 1.f;
    state__rasterization.polygonMode = vk::PolygonMode::eFill;
    state__rasterization.frontFace = vk::FrontFace::eClockwise;
    state__rasterization.cullMode = vk::CullModeFlagBits::eNone;
    state__rasterization.depthBiasEnable = false;

    state__multisampling.sampleShadingEnable = false;
    state__multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    state__multisampling.minSampleShading = 1.0f;

    color_blend_attachment
        .setColorBlendOp(vk::BlendOp::eAdd);

    state__color_blend.attachmentCount = 1;
    state__color_blend.pAttachments = &color_blend_attachment;

    for (const auto &description : frontend.render_target->render_passes)
    {
        // Create the pipeline
        pipeline_create_info
            .setStageCount(shader_stages.size())
            .setPStages(shader_stages.data())
            .setLayout(pipeline_layout)
            .setRenderPass(description.second->render_pass)
            .setSubpass(0)
            .setBasePipelineIndex(-1);

        const auto pipeline =
            hw.device->createGraphicsPipeline(nullptr, pipeline_create_info);

        pipelines.insert(std::make_pair( description.second->render_pass
                                       , pipeline
        ));
    }
}
