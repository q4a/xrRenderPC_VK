#ifndef DEVICE_MEMORY_H_
#define DEVICE_MEMORY_H_

#include <memory>

#include <vk_mem_alloc.h>

enum class BufferType
{
    Vertex,
    Index,
    Image
};

class DeviceBuffer
{
    const VmaAllocator *allocator_ref_;

public:
    explicit DeviceBuffer(const VmaAllocator *allocator);
    ~DeviceBuffer();

    VkBuffer            buffer;
    VmaAllocation       allocation;
    VmaAllocationInfo   allocation_info;
};

using BufferPtr = std::unique_ptr<DeviceBuffer>;

#endif // DEVICE_MEMORY_H_
