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

enum class ImageType
{
    Texture,
    Buffer,
    Depth
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
    friend class Hw;

    std::uint32_t layers_count; ///< Color layers
    std::uint32_t levels_count; ///< Mipmap levels
    vk::Extent3D  extent; ///< Image dimensions
    vk::Format    format; ///< Image format
    ImageType     type; ///< Type of image
public:
    explicit DeviceImage(const VmaAllocator *allocator);
    ~DeviceImage();

    vk::ImageView CreateView();

    VkImage image;
};

using ImagePtr = std::unique_ptr<DeviceImage>;

#endif // DEVICE_MEMORY_H_
