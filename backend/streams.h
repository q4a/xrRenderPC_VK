#ifndef BACKEND_STREAMS_H_
#define BACKEND_STREAMS_H_

#include <cstdint>

#include <gli/gli.hpp>

#include "device/memory.h"

struct BufferStride
{
    const std::size_t size_;
    void * const pointer_;

    explicit BufferStride( std::size_t size
                         , void *pointer
                         )
        : size_(size)
        , pointer_(pointer)
    {
    }
};


class Index
    : public BufferStride
{
    struct _stride_data_
    {
        std::uint16_t index;
    } stride_data_;
public:
    Index() = default;
    Index(std::uint16_t index)
        : BufferStride( sizeof(stride_data_)
                      , &stride_data_
          )
    {
        stride_data_.index = index;
    }

    void Set(std::uint16_t index)
    {
        stride_data_.index = index;
    }
    static const std::size_t size = sizeof(stride_data_);
};


struct StreamBuffer
{
    explicit StreamBuffer( std::size_t size
                         , BufferType type
                         );
    void Create();
    void Sync();
    void *GetHostPointer();

    BufferType  type_;
    BufferPtr   gpu_buffer_;
    BufferPtr   cpu_buffer_;

    std::size_t size_     = 0; ///< total buffer size
    std::size_t offset_   = 0; ///< absoulte offset of the open slice in buffer (dirty data)
    std::size_t position_ = 0; ///< relative offset for the current write
};


class StreamImage
{
public:
    explicit StreamImage(void *data, std::size_t size);
    void Sync();

    ImagePtr  gpu_image_;
    BufferPtr cpu_buffer_;

    vk::Extent3D extent; ///< Image dimensions
    vk::ImageView view; ///< Resource view
};


enum class StreamControl
{
    Reset,
    Flush,
    DiscardCache
};


template <class T>
class DataStream
    : public StreamBuffer
{
public:

    explicit DataStream<T>(std::size_t size);

    DataStream &operator <<(const BufferStride &&value)
    {
        const auto total_offset = offset_ + position_;
        VERIFY(total_offset + value.size_ <= size_);

        std::uint8_t *cpu_buffer_ptr =
            static_cast<std::uint8_t*>(cpu_buffer_->allocation_info.pMappedData);
        void * const dst_ptr = cpu_buffer_ptr + total_offset;
        void * const src_ptr = value.pointer_;
        memcpy( dst_ptr
              , src_ptr
              , value.size_
        );
        position_ += value.size_;
        return *this;
    }
    DataStream &operator <<(const StreamControl &code)
    {
        switch (code)
        {
        case StreamControl::Reset:
            offset_   = 0;
            position_ = 0;
            break;
        case StreamControl::Flush:
            Sync();
            break;
        case StreamControl::DiscardCache:
            position_ = 0;
            break;
        default:
            // Nothing here
            break;
        }
        return *this;
    }
    DataStream &operator >>(std::size_t &value)
    {
        value = offset_ + position_;
        return *this;
    }
};

struct VertexStream;
struct IndexStream;

#endif // BACKEND_STREAMS_H_
