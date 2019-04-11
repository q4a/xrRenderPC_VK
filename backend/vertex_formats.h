#ifndef BACKEND_VERTEX_FORMATS_H_
#define BACKEND_VERTEX_FORMATS_H_

#include "xrCore/_color.h"

#include "backend/streams.h"


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

namespace vertex_format
{
class TriangleList
    : public BufferStride
{
    struct _stride_data_
    {
        glm::vec4 position;
        glm::vec2 uv;
        glm::vec4 color;
    } stride_data_;
public:
    TriangleList()
        : BufferStride(sizeof(stride_data_), &stride_data_)
    {
    }

    /* Compatibility */
    inline void
    Set( float         x
       , float         y
       , std::uint32_t color
       , float         u
       , float         v
       )
    {
        stride_data_.position = { x, y, 0.0001f, 0.9999f };
        stride_data_.uv       = { u, v };

        // NOTE: The color value, as is, has color components
        //       swizzled. Need to restore normal order before
        //       passing this into vertex input stage.
        const auto color_corrected = bgr2rgb(color);
        const float r = color_get_R(color_corrected) / 255.0f;
        const float g = color_get_G(color_corrected) / 255.0f;
        const float b = color_get_B(color_corrected) / 255.0f;
        const float a = color_get_A(          color) / 255.0f;
        stride_data_.color = { r, g, b, a };
    }

    inline void
    Set( glm::vec4&& position
       , glm::vec2&& uv
       , glm::vec4&& color
       )
    {
        stride_data_.position = std::move(position);
        stride_data_.uv       = std::move(uv);
        stride_data_.color    = std::move(color);
    }
    static const std::size_t size = sizeof(stride_data_);
};
}

#endif // BACKEND_VERTEX_FORMATS_H_
