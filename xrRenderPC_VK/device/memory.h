#ifndef DEVICE_MEMORY_H_
#define DEVICE_MEMORY_H_

#include <memory>
#include <vk_mem_alloc.h>

enum class BufferType //TODO: replace by VK_BUFFER...
{
    Vertex,
    Index,
    Uniform,
    Image
};

struct DeviceAllocation
{
    explicit DeviceAllocation(const VmaAllocator *allocator);

    const VmaAllocator *allocator_ref_;
    VmaAllocation       allocation;
    VmaAllocationInfo   allocation_info;
};

class DeviceBuffer
    : public DeviceAllocation
{
public:
    explicit DeviceBuffer(const VmaAllocator *allocator);
    ~DeviceBuffer();

    VkBuffer buffer;
};

using BufferPtr = std::unique_ptr<DeviceBuffer>;


class DeviceImage
    : public DeviceAllocation
{
public:
    explicit DeviceImage(const VmaAllocator *allocator);
    ~DeviceImage();

    std::uint32_t layer_count; // image layers
    std::uint32_t level_count; // mipmap levels
    vk::Extent3D dimensions;
    VkImage image;
};

using ImagePtr = std::unique_ptr<DeviceImage>;

#endif // DEVICE_MEMORY_H_
