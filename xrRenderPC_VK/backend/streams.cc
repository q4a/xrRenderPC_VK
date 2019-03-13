#include "device/device.h"

#include "backend/streams.h"


/**
 *
 */
StreamBuffer::StreamBuffer
        ( std::size_t size
        , BufferType type
        )
    : size_(size)
    , type_(type)
{
}


/**
 *
 */
void
StreamBuffer::Create()
{
    cpu_buffer_ = hw.CreateCpuBuffer(size_);
    gpu_buffer_ = hw.CreateGpuBuffer(size_, type_);
}


/**
 *
 */
void
StreamBuffer::Sync()
{
    R_ASSERT(offset_ + position_ <= size_);

    const auto transfer_size = size_ - offset_ + position_;
    if (transfer_size == 0)
    {
        return;
    }

    hw.Transfer( gpu_buffer_
               , cpu_buffer_
               , offset_
               , transfer_size
    );

    offset_  += position_;
    position_ = 0;
}


/*!
 * \brief   The only image stream class constructor
 *
 * \param [in] data pointer to image data
 * \param [in] size size of image in memory
 */
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

    gpu_image_ = hw.CreateGpuImage(texture);
}


/*!
 * \brief   Transfers host data into device memory
 */
void
StreamImage::Sync()
{
    hw.Transfer(gpu_image_, cpu_buffer_);
}


/**
 *
 */
template <>
DataStream<VertexStream>::DataStream
        ( std::size_t size
        )
    : StreamBuffer( size
                  , BufferType::Vertex
      )
{
}


/**
 *
 */
template <>
DataStream<IndexStream>::DataStream
        ( std::size_t size
        )
    : StreamBuffer( size
                  , BufferType::Index
      )
{
}
