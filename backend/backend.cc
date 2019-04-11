/*!
 * \file    backend.cc
 * \brief   Render backend implementation
 */

#include "device/device.h"
#include "backend/backend.h"


constexpr std::size_t kilobyte = 1024;
constexpr std::size_t operator"" _Kb
        ( unsigned long long value
        )
{
    return value * kilobyte;
}

constexpr std::size_t vertex_buffer_size = 4096_Kb;
constexpr std::size_t index_buffer_size  =  512_Kb;

/* Quad index cache */
constexpr std::size_t triangles_num         = 4096;
constexpr std::size_t index_count           = triangles_num
                                              * triangles_per_quad
                                              * vertices_per_triangle;

constexpr std::size_t index_cache_size      = index_count * Index::size;


//-----------------------------------------------------------------------------
BackEnd::BackEnd()
    : vertex_stream(vertex_buffer_size)
    , index_stream(index_buffer_size)
    , index_cache(index_cache_size)
{
}


//-----------------------------------------------------------------------------
void
BackEnd::CreateCommandBuffers()
{
    const auto cmd_pool_create_info = vk::CommandPoolCreateInfo()
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(hw.graphicsQfamilyIdx);

    cmd_pool_ = hw.device->createCommandPoolUnique(cmd_pool_create_info);

    const auto cmd_buffers_count = hw.baseRt.size();
    const auto cmd_buffer_alloc_info = vk::CommandBufferAllocateInfo()
        .setCommandPool(cmd_pool_.get())
        .setCommandBufferCount(cmd_buffers_count)
        .setLevel(vk::CommandBufferLevel::ePrimary);

    draw_cmd_buffers_ =
        hw.device->allocateCommandBuffersUnique(cmd_buffer_alloc_info);
}


//-----------------------------------------------------------------------------
void
BackEnd::OnDeviceCreate()
{
    CreateCommandBuffers();

    CreateIndexCache();

    // create semaphores
    frame_semaphores.resize(hw.baseRt.size());
    for (auto &semaphore : frame_semaphores)
    {
        semaphore = hw.device->createSemaphoreUnique({});
    }

    render_semaphores.resize(hw.baseRt.size());
    for (auto &semaphore : render_semaphores)
    {
        semaphore = hw.device->createSemaphoreUnique({});
    }

    vertex_stream.Create();
    index_stream.Create();
}


//-----------------------------------------------------------------------------
void
BackEnd::DestroyCommandBuffers()
{
    for (auto &buffer : draw_cmd_buffers_)
    {
        buffer.reset();
    }

    cmd_pool_.reset();
}


//-----------------------------------------------------------------------------
void
BackEnd::OnDeviceDestroy()
{
    DestroyCommandBuffers();
}


//-----------------------------------------------------------------------------
void
BackEnd::CreateIndexCache()
{
    index_cache.Create();

    for ( auto triangle_idx = 0
        ; triangle_idx < triangles_num
        ; triangle_idx++
        )
    {
        const auto start_vertex =
            triangle_idx * (vertices_per_triangle + 1);

        index_cache << Index{ std::uint16_t(start_vertex + 0) };
        index_cache << Index{ std::uint16_t(start_vertex + 1) };
        index_cache << Index{ std::uint16_t(start_vertex + 2) };

        index_cache << Index{ std::uint16_t(start_vertex + 3) };
        index_cache << Index{ std::uint16_t(start_vertex + 2) };
        index_cache << Index{ std::uint16_t(start_vertex + 1) };
    }

    index_cache << StreamControl::Sync;
}
