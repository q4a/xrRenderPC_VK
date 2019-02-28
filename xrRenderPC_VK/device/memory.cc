#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "device/device.h"
#include "device/memory.h"


/**
 *
 */
DeviceBuffer::DeviceBuffer
        ( const VmaAllocator *allocator
        )
    : allocator_ref_(allocator)
{
}


/**
 *
 */
DeviceBuffer::~DeviceBuffer()
{
    vmaDestroyBuffer(*allocator_ref_, buffer, allocation);
}


/**
 *
 */
buffer_ptr
Hw::CreateCpuBuffer
        ( std::size_t size
        ) const
{
    buffer_ptr buffer =
        std::unique_ptr<DeviceBuffer>(new DeviceBuffer{ &allocator_ });

    VkBufferCreateInfo buffer_create_info{};
    buffer_create_info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size        = size;
    buffer_create_info.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_create_info{};
    alloc_create_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    alloc_create_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    vmaCreateBuffer( allocator_
                   , &buffer_create_info
                   , &alloc_create_info
                   , &buffer->buffer
                   , &buffer->allocation
                   , &buffer->allocation_info
    );

    return buffer;
}


/**
 *
 */
buffer_ptr
Hw::CreateGpuBuffer
        ( std::size_t size
        ) const
{
    buffer_ptr buffer =
        std::unique_ptr<DeviceBuffer>(new DeviceBuffer{ &allocator_ });

    VkBufferCreateInfo buffer_create_info{};
    buffer_create_info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size        = size;
    buffer_create_info.usage       = VK_BUFFER_USAGE_TRANSFER_DST_BIT
                                   | VK_BUFFER_USAGE_INDEX_BUFFER_BIT; // FIXME
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_create_info{};
    alloc_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    vmaCreateBuffer( allocator_
                   , &buffer_create_info
                   , &alloc_create_info
                   , &buffer->buffer
                   , &buffer->allocation
                   , nullptr
    );

    return buffer;
}


/**
 *
 */
void
Hw::Transfer
        ( buffer_ptr &dst
        , buffer_ptr &src
        , std::size_t offset
        , std::size_t size
        ) const
{
    const auto buffer_begin_info = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    const auto region = vk::BufferCopy()
        .setSrcOffset(offset)
        .setDstOffset(offset)
        .setSize(size);

    vk::Buffer src_buffer(src->buffer);
    vk::Buffer dst_buffer(dst->buffer);

    /* Fill command buffer */
    const std::size_t buffer_idx = 0;
    const auto &cmd_buffer = hw.ctrl_cmd_buf_[buffer_idx];

    cmd_buffer->begin(buffer_begin_info);
    cmd_buffer->copyBuffer(src_buffer, dst_buffer, region);
    cmd_buffer->end();

    /* Submit */
    const auto submit_info = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmd_buffer.get());

    hw.submission_q.submit(submit_info, {});
    hw.submission_q.waitIdle();
}
