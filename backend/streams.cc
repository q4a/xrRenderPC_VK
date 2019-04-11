#include "device/device.h"

#include "backend/streams.h"


//-----------------------------------------------------------------------------
StreamBuffer::StreamBuffer
        ( std::size_t size
        , BufferType type
        )
    : size_(size)
    , type_(type)
{
}


//-----------------------------------------------------------------------------
void
StreamBuffer::Create()
{
    cpu_buffer_ = hw.CreateCpuBuffer(size_);
    gpu_buffer_ = hw.CreateGpuBuffer(size_, type_);
}


//-----------------------------------------------------------------------------
void
StreamBuffer::Sync()
{
    const auto transfer_size = offset_ + position_;
    VERIFY(transfer_size <= size_);

    if (transfer_size == 0) // no data
    {
        return;
    }

    hw.Transfer( gpu_buffer_
               , cpu_buffer_
               , 0 // offset
               , transfer_size
    );

    offset_   = 0;
    position_ = 0;
}


//-----------------------------------------------------------------------------
void *
StreamBuffer::GetHostPointer()
{
    return cpu_buffer_->allocation_info.pMappedData;
}


//-----------------------------------------------------------------------------
StreamImage::StreamImage
        ( void        *data
        , std::size_t  size
        )
{
    const auto &texture = gli::load(static_cast<char *>(data), size);

    cpu_buffer_ = hw.CreateCpuBuffer(texture.size());

    // copy the data into staging buffer
    CopyMemory( cpu_buffer_->allocation_info.pMappedData
              , texture.data()
              , texture.size()
    );

    // TODO: already saved in `image` object
    extent.width  = texture.extent().x;
    extent.height = texture.extent().y;
    extent.depth  = texture.extent().z;

    gpu_image_ = hw.CreateGpuImage(std::move(texture));
    view       = gpu_image_->CreateView();
}


//-----------------------------------------------------------------------------
StreamImage::StreamImage
        ( const vk::Extent3D &dimensions
        , vk::Format          format /* = vk::Format::eR8G8B8A8Unorm */
        , std::uint32_t       layers_count /* = 1 */
        , std::uint32_t       levels_count /* = 1 */
        )
{
    extent = dimensions;

    constexpr auto texel_size = 4; // TODO: this is for R8G8B8A8
    const auto image_size = extent.width * extent.height * texel_size;

    cpu_buffer_ = hw.CreateCpuBuffer(image_size);
    gpu_image_  = hw.CreateGpuImage( dimensions
                                   , format
                                   , layers_count
                                   , levels_count
                                   );
    view        = gpu_image_->CreateView();
}


//-----------------------------------------------------------------------------
void
StreamImage::Sync()
{
    hw.Transfer(gpu_image_, cpu_buffer_);
}


//-----------------------------------------------------------------------------
template <>
DataStream<VertexStream>::DataStream
        ( std::size_t size
        )
    : StreamBuffer(size, BufferType::Vertex)
{
}


//-----------------------------------------------------------------------------
template <>
DataStream<IndexStream>::DataStream
        ( std::size_t size
        )
    : StreamBuffer(size, BufferType::Index)
{
}
