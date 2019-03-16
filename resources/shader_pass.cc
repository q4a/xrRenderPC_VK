#include "device/device.h"
#include "resources/manager.h"

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
static vk::PipelineColorBlendStateCreateInfo state__color_blend =
{};
static vk::PipelineDynamicStateCreateInfo state__dynamic =
{};

/*!
 * Pipeline shared stencils
 */
vk::PipelineDepthStencilStateCreateInfo state__depth_stencil =
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
    nullptr,
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
ShaderPass::MergeConstants
        ( const std::map<std::string, ShaderResource> &shader_resources
        )
{
    for (const auto &[name, constant] : shader_resources)
    {
        const auto &iterator = resources.find(name);
        if (iterator == resources.cend())
        {
            resources.insert(std::make_pair(name, constant));
        }
        else
        {
            R_ASSERT(false);
        }
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
            .setBinding(resource.second.binding)
            .setDescriptorType(resource.second.type)
            .setDescriptorCount(1)
            .setStageFlags(resource.second.stage);

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

    auto i = 0;
    for (const auto &resource : resources)
    {
        pool_sizes[i].type = resource.second.type;
        pool_sizes[i].descriptorCount = 1; // TODO: need to count
        i++;
    }

    const auto pool_create_info = vk::DescriptorPoolCreateInfo()
        .setMaxSets(1)
        .setPPoolSizes(pool_sizes.data())
        .setPoolSizeCount(pool_sizes.size());
    auto descriptor_pool = hw.device->createDescriptorPool(pool_create_info);

    const auto &layout = descriptors_layout; // for const cast
    const auto descriptor_set_allocate_info = vk::DescriptorSetAllocateInfo()
        .setDescriptorPool(descriptor_pool)
        .setDescriptorSetCount(1)
        .setPSetLayouts(&layout);
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
        .setHeight(hw.draw_rect.height);
    // TODO: max depth!

    const auto scissor = vk::Rect2D()
        .setExtent(hw.draw_rect);

    state__viewport
        .setViewportCount(1)
        .setPViewports(&viewport)
        .setScissorCount(1)
        .setPScissors(&scissor);

    // Create the pipeline
    pipeline_create_info
        .setStageCount(shader_stages.size())
        .setPStages(shader_stages.data())
        .setLayout(pipeline_layout)
        .setRenderPass(/*frontend.render_pass_.get()*/nullptr)
        .setSubpass(0) // This one is hardest to identify
        .setBasePipelineIndex(-1);

    pipeline =
        hw.device->createGraphicsPipeline(nullptr, pipeline_create_info);
}
