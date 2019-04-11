/*
 * \file    streams.h
 * \brief   Data management interfaces definition
 */

#ifndef BACKEND_STREAMS_H_
#define BACKEND_STREAMS_H_

#include <cstdint>

#include <gli/gli.hpp>

#include "backend/vertex_formats.h"
#include "device/memory.h"


struct StreamBuffer
{
    explicit StreamBuffer( std::size_t size
                         , BufferType type
                         );
    void Create();
    void Sync();
    /*!
     * \brief   Returns pointer to host mapped data
     */
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
    /*!
     * \brief   Creates an image buffer from encoded DDS data in memory
     */
    StreamImage( void       *data ///< [in] pointer to data
               , std::size_t size ///< [in] data size
               );

    /*!
     * \brief   Creates a raw image buffer
     *
     * Currently used for Theora video streams
     */
    StreamImage( const vk::Extent3D &dimensions ///< [in] image extent
               , vk::Format          format = vk::Format::eR8G8B8A8Unorm ///< [in] image format
               , std::uint32_t       layers_count = 1 ///< [in] color array levels
               , std::uint32_t       levels_count = 1 ///< [in] mipmap levels
               );

    /*!
     * \brief   Transfers host data into device memory
     */
    void Sync();

    ImagePtr  gpu_image_;
    BufferPtr cpu_buffer_;

    vk::Extent3D extent; ///< Image dimensions
    vk::ImageView view; ///< Resource view
};


/*!
 * \brief   Data stream control operations
 */
enum class StreamControl
{
    Reset,          ///< Reset stream's pointers
    Flush,          ///< Advance offset to current position
    Sync,           ///< Upload data to device (has implicit `Flush`)
    DiscardCache    ///< Rollback data pointer to initial offset
};


/*!
 * \brief   Geometry stream class
 */
template <class T>
class DataStream
    : public StreamBuffer
{
public:
    explicit DataStream<T>(std::size_t size);

    DataStream &operator <<(const BufferStride &value)
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
            offset_  += position_;
            position_ = 0;
            break;
        case StreamControl::Sync:
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


/*!
 * \brief   Data stream specialization types
 */
struct VertexStream;
struct IndexStream;

#endif // BACKEND_STREAMS_H_
