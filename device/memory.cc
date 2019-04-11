#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "device/device.h"
#include "device/memory.h"


/*!
 * \brief   The only constructor of base allocation class
 */
DeviceAllocation::DeviceAllocation
        ( const VmaAllocator *allocator
        )
    : allocator_ref_(allocator)
{
    // Nothing here
}


/*!
 * \brief   The only constructor of buffer allocation class
 *
 * Bypasses allocation pointer to base class
 */
DeviceBuffer::DeviceBuffer
        ( const VmaAllocator *allocator
        )
    : DeviceAllocation(allocator)
{
    // Nothing here
}


/*!
 * \brief   Destructor of buffer allocation class
 */
DeviceBuffer::~DeviceBuffer()
{
    vmaDestroyBuffer(*allocator_ref_, buffer, allocation);
}


/*!
 * \brief   The only constructor of image allocation class
 *
 * Bypasses allocation pointer to base class
 */
DeviceImage::DeviceImage
        ( const VmaAllocator *allocator
        )
    : DeviceAllocation(allocator)
{
    // Nothing here
}


/*!
 * \brief   Destructor of image allocation class
 */
DeviceImage::~DeviceImage()
{
    vmaDestroyImage(*allocator_ref_, image, allocation);
}


/*!
 * \brief   Creates image view for the resource
 */
vk::ImageView
DeviceImage::CreateView()
{
    // Create image view
    auto &subresource_range = vk::ImageSubresourceRange()
        .setLayerCount(layers_count)
        .setLevelCount(levels_count);

    switch (type)
    {
    case ImageType::Buffer:
        // no break here
    case ImageType::Texture:
        subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
        break;
    case ImageType::Depth:
        subresource_range.setAspectMask(vk::ImageAspectFlagBits::eDepth);
        break;
    default:
        FATAL("Invalid image type");
    }

    auto components_mapping = vk::ComponentMapping();

    if (format == vk::Format::eR8Unorm)
    {
        components_mapping
            .setA(vk::ComponentSwizzle::eR)
            .setR(vk::ComponentSwizzle::eOne)
            .setG(vk::ComponentSwizzle::eOne)
            .setB(vk::ComponentSwizzle::eOne);
    }

    const auto &view_create_info = vk::ImageViewCreateInfo()
        .setImage(vk::Image{ image })
        .setViewType(vk::ImageViewType::e2D) // TODO
        .setFormat(format)
        .setSubresourceRange(subresource_range)
        .setComponents(components_mapping);

    const auto view = hw.device->createImageView(view_create_info);
    return view;
}


//-----------------------------------------------------------------------------
BufferPtr
Hw::CreateCpuBuffer
        ( std::size_t size
        ) const
{
    BufferPtr buffer =
        std::unique_ptr<DeviceBuffer>{ new DeviceBuffer{ &allocator_ } };

    VkBufferCreateInfo buffer_create_info{};
    buffer_create_info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size        = size;
    buffer_create_info.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_create_info{};
    alloc_create_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    alloc_create_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    auto result = vmaCreateBuffer( allocator_
                                 , &buffer_create_info
                                 , &alloc_create_info
                                 , &buffer->buffer
                                 , &buffer->allocation
                                 , &buffer->allocation_info
    );
    VERIFY(result == VK_SUCCESS);

    return buffer;
}


//-----------------------------------------------------------------------------
BufferPtr
Hw::CreateGpuBuffer
        ( std::size_t size
        , BufferType  type
        ) const
{
    BufferPtr buffer =
        std::unique_ptr<DeviceBuffer>{ new DeviceBuffer { &allocator_} };

    VkBufferCreateInfo buffer_create_info{};
    buffer_create_info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size        = size;
    buffer_create_info.usage       = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    switch (type)
    {
    case BufferType::Vertex:
        buffer_create_info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
    case BufferType::Index:
        buffer_create_info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        break;
    case BufferType::Uniform:
        buffer_create_info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        break;
    default:
        R_ASSERT(0);
    }

    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_create_info{};
    alloc_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    auto result = vmaCreateBuffer( allocator_
                                 , &buffer_create_info
                                 , &alloc_create_info
                                 , &buffer->buffer
                                 , &buffer->allocation
                                 , nullptr
    );
    VERIFY(result == VK_SUCCESS);

    return buffer;
}


//-----------------------------------------------------------------------------
void
Hw::Transfer
        ( const BufferPtr &destination
        , const BufferPtr &source
        , std::size_t      offset
        , std::size_t      size
        ) const
{
    const auto &buffer_begin_info = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    const auto &region = vk::BufferCopy()
        .setSrcOffset(offset)
        .setDstOffset(offset)
        .setSize(size);

    // Start command buffer recording
    const std::size_t buffer_idx = 0;
    const auto &cmd_buffer = hw.ctrl_cmd_buf_[buffer_idx];

    cmd_buffer->begin(buffer_begin_info);
    cmd_buffer->copyBuffer( vk::Buffer{ source->buffer }
                          , vk::Buffer{ destination->buffer }
                          , region
    );
    cmd_buffer->end();

    // Submit the buffer
    const auto submit_info = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmd_buffer.get());

    hw.submission_q.submit(submit_info, {});
    hw.submission_q.waitIdle();
}


//-----------------------------------------------------------------------------
ImagePtr
Hw::CreateGpuImage
        ( const gli::texture &image_description
        ) const
{
    vk::Extent3D dimensions;
    dimensions.width  = image_description.extent().x;
    dimensions.height = image_description.extent().y;
    dimensions.depth  = image_description.extent().z;

    auto format = vk::Format{ image_description.format() };
    if (image_description.format() == gli::FORMAT_A8_UNORM_PACK8)
    {
		format = vk::Format::eR8Unorm;
    }

    return CreateGpuImage( dimensions
                         , format
                         , image_description.layers()
                         , image_description.levels()
    );
}


//-----------------------------------------------------------------------------
ImagePtr
Hw::CreateGpuImage
        ( const vk::Extent3D &dimensions
        , vk::Format          format
        , std::uint32_t       layers_count
        , std::uint32_t       levels_count
        , ImageType           type /* = ImageType::Texture */
        ) const
{
    ImagePtr image =
        std::unique_ptr<DeviceImage>{ new DeviceImage { &allocator_ } };

    image->extent       = dimensions;
    image->layers_count = layers_count;
    image->levels_count = levels_count;
    image->format       = format;
    image->type         = type;

    VkImageCreateInfo image_create_info{};
    image_create_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.extent        = image->extent;
    image_create_info.imageType     = VK_IMAGE_TYPE_2D; // TODO: take it from metadata
    image_create_info.mipLevels     = image->levels_count;
    image_create_info.arrayLayers   = image->layers_count;
    image_create_info.format        = (VkFormat)image->format;
    image_create_info.tiling        = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    switch (image->type)
    {
    case ImageType::Texture:
        image_create_info.usage     = VK_IMAGE_USAGE_TRANSFER_DST_BIT
                                    | VK_IMAGE_USAGE_SAMPLED_BIT;
        break;
    case ImageType::Buffer:
        image_create_info.usage     = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                                    | VK_IMAGE_USAGE_SAMPLED_BIT;
        break;
    case ImageType::Depth:
        image_create_info.usage     = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        break;
    default:
        R_ASSERT(0);
    }

    image_create_info.samples       = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_create_info{};
    alloc_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    auto result = vmaCreateImage( allocator_
                                , &image_create_info
                                , &alloc_create_info
                                , &image->image
                                , &image->allocation
                                , nullptr
    );
    VERIFY(result == VK_SUCCESS);

    return image;
}


//-----------------------------------------------------------------------------
void
Hw::Transfer
        ( ImagePtr        &destination
        , const BufferPtr &source
        )
{
    const auto &buffer_begin_info = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    const auto &subresource_layout = vk::ImageSubresourceLayers()
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setLayerCount(destination->layers_count);

    const auto &image_region = vk::BufferImageCopy()
        .setImageSubresource(subresource_layout)
        .setImageExtent(destination->extent);

    // Start command buffer recording
    const std::size_t buffer_idx = 0;
    const auto &cmd_buffer = hw.ctrl_cmd_buf_[buffer_idx];

    cmd_buffer->begin(buffer_begin_info);

    // Change image layout to transfer
    const auto &subresource_range = vk::ImageSubresourceRange()
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setLevelCount(destination->levels_count)
        .setLayerCount(destination->layers_count);

    auto &memory_barrier = vk::ImageMemoryBarrier()
        .setSubresourceRange(subresource_range)
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
        .setImage(vk::Image{ destination->image })
        .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

    cmd_buffer->pipelineBarrier( vk::PipelineStageFlagBits::eTopOfPipe
                               , vk::PipelineStageFlagBits::eTransfer
                               , {} // no dependencies
                               , {} // no barriers
                               , {} // no buffer barriers
                               , { memory_barrier }
    );

    // Copy image data
    cmd_buffer->copyBufferToImage( vk::Buffer{ source->buffer }
                                 , vk::Image{ destination->image }
                                 , vk::ImageLayout::eTransferDstOptimal
                                 , { image_region }
    );

    // Change image layout to shader read
    memory_barrier
        .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

    cmd_buffer->pipelineBarrier( vk::PipelineStageFlagBits::eTransfer
                               , vk::PipelineStageFlagBits::eFragmentShader
                               , {} // no dependencies
                               , {} // no barriers
                               , {} // no buffer barriers
                               , { memory_barrier }
    );

    cmd_buffer->end();

    // Submit the buffer
    const auto submit_info = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmd_buffer.get());

    hw.submission_q.submit(submit_info, {});
    hw.submission_q.waitIdle();
}
