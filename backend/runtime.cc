#include <vector>

#include "device/device.h"
#include "frontend/render.h"
#include "backend/streams.h"

#include "backend/backend.h"

/**
 *
 */
void
BackEnd::OnFrameBegin
        ( std::uint32_t frame_index
        )
{
    vertex_stream << StreamControl::Reset;
    index_stream  << StreamControl::Reset;

    auto& cmd_buffer = draw_cmd_buffers_[frame_index];
    auto& swapchain_resource = hw.baseRt[frame_index];

    const auto begin_info = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
    
    cmd_buffer->begin(begin_info);

    std::array<vk::ClearValue, 1> clear_values;
    clear_values[0].color = vk::ClearColorValue(std::array<float, 4>{0.33f, 0.33f, 0.10f, 1.0f});

    // Begin render pass
    const auto renderpass_begin_info = vk::RenderPassBeginInfo()
        .setRenderPass(render_pass.get())
        .setFramebuffer(swapchain_resource.frameBuffer)
        .setRenderArea(vk::Rect2D( vk::Offset2D(0, 0)
                                 , vk::Extent2D( hw.draw_rect.width
                                               , hw.draw_rect.height
        )))
        .setClearValueCount(clear_values.size())
        .setPClearValues(clear_values.data());

    cmd_buffer->beginRenderPass( renderpass_begin_info
                               , vk::SubpassContents::eInline
    );

    state.frame_index = frame_index;
}


/**
 *
 */
void
BackEnd::OnFrameEnd
        ( std::uint32_t frame_index
        )
{
    R_ASSERT(frame_index == state.frame_index);

    auto& cmd_buffer = draw_cmd_buffers_[frame_index];

    cmd_buffer->endRenderPass();
    cmd_buffer->end();

    const vk::PipelineStageFlags wait_mask =
        vk::PipelineStageFlagBits::eTransfer;
    const auto submit_info = vk::SubmitInfo()
        .setWaitSemaphoreCount(1)
        .setPWaitDstStageMask(&wait_mask)
        .setPWaitSemaphores(&frame_semaphores[frame_index].get())
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmd_buffer.get())
        .setSignalSemaphoreCount(1)
        .setPSignalSemaphores(&render_semaphores[frame_index].get());

    hw.submission_q.submit(submit_info, {});
}


std::shared_ptr<Texture>
BackEnd::GetActiveTexture
        ( std::size_t index
        ) const
{
    VERIFY(state.pass);
    VERIFY(index < state.pass->textures.size());

    // TODO: textures in the map are sorted by name
    //       not sure what exactly index means from
    //       case to case
    auto iterator = state.pass->textures.cbegin();
    const auto &texture = std::next(iterator, index);

    return texture->second;
}


/*!
 * \brief   Updates descriptors for current pipeline
 */
void
BackEnd::UpdateDescriptors()
{
    VERIFY(state.pass);

    std::vector<vk::WriteDescriptorSet> update_info(state.pass->resources.size());
    vk::DescriptorBufferInfo buffer_info;
    vk::DescriptorImageInfo  image_info;
    vk::DescriptorImageInfo  sampler_info;

    auto i = 0;
    for (const auto &[name, resource] : state.pass->resources)
    {
        const auto descriptor_type = resource->type;

        auto &desc_write = update_info[i];
        desc_write.dstSet          = state.pass->descriptors[state.frame_index];
        desc_write.dstBinding      = resource->binding;
        desc_write.descriptorType  = descriptor_type;
        desc_write.descriptorCount = 1;

        switch (descriptor_type)
        {
        case vk::DescriptorType::eUniformBuffer:
            {
                // TODO: make this more elegant
                auto &constant_table =
                    static_cast<ConstantTable&>(const_cast<ShaderResource &>(*resource));
                auto &buffer = constant_table.buffers[state.frame_index];

                // Update UBO content
                for (auto &[name, member] : constant_table.members)
                {
                    member.Update();
                }

                buffer_info.buffer =
                    vk::Buffer(buffer->gpu_buffer_->buffer);
                buffer_info.range  = constant_table.size;

                desc_write.pBufferInfo = &buffer_info;
            }
            break;
        case vk::DescriptorType::eSampledImage:
            {
                const auto &iterator = state.pass->textures.find(name);
                VERIFY(iterator != state.pass->textures.cend());

                const auto &texture = iterator->second;

                texture->Loader();

                image_info.imageLayout =
                    vk::ImageLayout::eShaderReadOnlyOptimal;
                image_info.imageView = texture->image->view;

                desc_write.pImageInfo = &image_info;
            }
            break;
        case vk::DescriptorType::eSampler:
            {
                const auto &iterator = state.pass->samplers.find(name);
                VERIFY(iterator != state.pass->samplers.cend());

                const auto &sampler = iterator->second;

                vk::DescriptorImageInfo info;
                sampler_info.imageLayout =
                    vk::ImageLayout::eShaderReadOnlyOptimal;
                sampler_info.sampler = sampler.sampler;

                desc_write.pImageInfo = &sampler_info;
            }
            break;
        }

        i++;
    }

    hw.device->updateDescriptorSets(update_info, {});
}


void
BackEnd::SetShader
        ( const std::shared_ptr<Shader> &shader
        )
{
    // Update render state
    const auto &pass = shader->elements[0]->shader_passes[0];

    if (state.pass == pass)
    {
        // Nothing to do. The pipeline is already binded.
        return;
    }
    state.pass = pass;

    // In case when the descriptors set has been binded before
    // the next descriptors update will break the command
    // buffer. For such case do update only once a frame.
	if (pass->frame_when_updated != Device.dwFrame)
	{
		UpdateDescriptors();
		pass->frame_when_updated = Device.dwFrame;
	}

    auto &cmd_buffer = draw_cmd_buffers_[state.frame_index];

    // set shaders
    cmd_buffer->bindPipeline( vk::PipelineBindPoint::eGraphics
                            , pass->pipeline
    );

    cmd_buffer->bindDescriptorSets( vk::PipelineBindPoint::eGraphics
                                  , pass->pipeline_layout
                                  , 0
                                  , pass->descriptors[state.frame_index]
                                  , {}
    );
}

void
BackEnd::SetGeometry
        ( DataStream<VertexStream> &vertices
        , DataStream<IndexStream>  &indices
        )
{
    BindVertexBuffer(vertices);
    BindIndexBuffer(indices);
}


void
BackEnd::SetGeometry
        ( DataStream<VertexStream> &vertices
        )
{
    BindVertexBuffer(vertices);
}


/*!
 * \brief   Binds vertex buffer to pipeline
 * \param [in] vertices vertex stream to bind
 *
 * Flushes vertex buffer before binding and binds it starting from offset
 * set by previous flush. The function updates render state.
 */
void
BackEnd::BindVertexBuffer
        ( DataStream<VertexStream> &vertices
        )
{
    // Update render state
    state.vertices = &vertices;

    // Flush vertex buffer
    const auto vertices_offset = vertices.offset_;
    vertices << StreamControl::Flush;

    // Bind it!
    auto &cmd_buffer   = draw_cmd_buffers_[state.frame_index];
    auto vertex_buffer = vk::Buffer(vertices.gpu_buffer_->buffer);
    cmd_buffer->bindVertexBuffers( 0 // first binding
                                 , { vertex_buffer }
                                 , { vertices_offset }
    );
}


/*!
 * \brief   Binds index buffer to pipeline
 * \param [in] indices index stream to bind
 *
 * Flushes index buffer before binding and binds it starting from offset
 * set by previous flush. The function updates render state.
 */
void
BackEnd::BindIndexBuffer
        ( DataStream<IndexStream> &indices
        )
{
    state.indices = &indices;


    const auto indices_offset = indices.offset_;
    indices << StreamControl::Flush;

    auto &cmd_buffer  = draw_cmd_buffers_[state.frame_index];
    auto index_buffer = vk::Buffer(indices.gpu_buffer_->buffer);
    cmd_buffer->bindIndexBuffer( index_buffer
                               , indices_offset
                               , vk::IndexType::eUint16
    );
}


/*!
 * \brief   Draws triangles list
 * \param [in] primitives_count amount of triangles in binded vertex buffer
 *
 * TODO: Need to support TS, lines and other topologies
 */
void
BackEnd::Draw(std::uint32_t primitives_count)
{
    auto &cmd_buffer = draw_cmd_buffers_[state.frame_index];
    cmd_buffer->draw( primitives_count * vertices_per_triangle
                    , primitives_count
                    , 0 // first vertex
                    , 0 // first instance
    );
}


/*!
 * \brief   Draws indexed triangles list
 * \param [in] primitives_count amount of triangles in binded vertex buffer
 *
 * TODO: Need to support TS, lines and other topologies
 */
void
BackEnd::DrawIndexed(std::uint32_t primitives_count)
{
    auto &cmd_buffer = draw_cmd_buffers_[state.frame_index];
    cmd_buffer->drawIndexed( primitives_count * vertices_per_triangle // amount of index elements
                           , primitives_count
                           , 0 // first index
                           , 0 // vertex offset
                           , 0 // first instance
    );
}
