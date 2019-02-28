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
{
    cpu_buffer_ = hw.CreateCpuBuffer(size);
    gpu_buffer_ = hw.CreateGpuBuffer(size, type);
}


/**
 *
 */
void
StreamBuffer::Sync()
{
    R_ASSERT(offset_ + position_ <= size_);

    const auto transfer_size = size_ - offset_ + position_;

    hw.Transfer( gpu_buffer_
               , cpu_buffer_
               , offset_
               , transfer_size
    );

    offset_  += position_;
    position_ = 0;
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
