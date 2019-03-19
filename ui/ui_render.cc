/*!
 * \file    ui_render.cc
 * \brief   UI renderer interface implementation
 */

#include "backend/backend.h"
#include "backend/vertex_formats.h"
#include "device/device.h"
#include "factory/ui/shader.h"

#include "ui/ui_render.h"

UiRender ui;

/*!
 * \brief   Stub implementation
 */
void
UiRender::CreateUIGeom()
{
    // Nothing here
}


/*!
 * \brief   Stub implementation
 */
void
UiRender::DestroyUIGeom()
{
    // Nothing here
}


/*!
 * \brief   Stub implementation
 */
void
UiRender::SetAlphaRef
        ( int alpha_reference
        )
{
    Msg("! %s: not implemented", __FUNCTION__);
}


/*!
 * \brief   Stub  implementation
 */
void
UiRender::SetScissor
        ( Irect *area /* = nullptr */
        )
{
    Msg("! %s: not implemented", __FUNCTION__);
}


/*!
 * \brief   Stub implementation
 */
void
UiRender::CacheSetCullMode
        ( CullMode cull_mode
        )
{
    Msg("! %s: not implemented", __FUNCTION__);
}


/*!
 * \brief   Stub implementation
 */
void
UiRender::CacheSetXformWorld
        ( const Fmatrix &matrix
        )
{
    Msg("! %s: not implemented", __FUNCTION__);
}

/*!
 * \brief   Asks backend to bind rendering pipeline
 * \param [in] shader   Assotiated UI shader object
 */
void
UiRender::SetShader
        ( IUIShader &shader
        )
{
    const auto &ui_shader = dynamic_cast<fUIShader&>(shader);
    backend.SetShader(ui_shader.shader_);
}


/*!
 * \brief   Returns UI element's texture extent
 * \param [out] dimensions  2D vector with texture dimensions
 */
void
UiRender::GetActiveTextureResolution
        ( Fvector2 &dimensions
        )
{
    const auto &texture = backend.GetActiveTexture(0);
    dimensions.x = texture->image->extent.width;
    dimensions.y = texture->image->extent.height;
}


/*!
 * \brief   Adds vertex into vertex buffer
 * \param [in] x     X coordinate
 * \param [in] y     Y coordinate
 * \param [in] z     Z coordinate (discarded)
 * \param [in] color vertex color
 * \param [in] u     texture U coordinate
 * \param [in] v     texture V coordinate
 */
void
UiRender::PushPoint
        ( float x
        , float y
        , float z
        , u32   color
        , float u
        , float v
        )
{
    switch (point_type_)
    {
    case pttTL:
        {
            vertex_format::TriangleList vertex;

            const auto y_corrected = float(hw.draw_rect.height) - y;
            vertex.Set( x
                      , y_corrected
                      , color
                      , u
                      , v
            );
            backend.vertex_stream << (BufferStride)vertex;
        }
        break;
    default:
        R_ASSERT(false);
    }
}


/*!
 * \brief   Starts vertex buffer recording
 * \param [in] max_vertices     Expected amount of vertices
 * \param [in] primitive_type   Desired pipeline primitive topology
 * \param [in] point_type       Vertex input format
 */
void
UiRender::StartPrimitive
        ( u32            max_vertices
        , ePrimitiveType primitive_type
        , ePointType     point_type
        )
{
    VERIFY(primitive_type_ == ptNone);
    VERIFY(point_type_ == pttNone);

    /* Initialize render state
     */
    primitive_type_ = primitive_type;
    point_type_     = point_type;
    vertices_count_ = max_vertices;

    backend.vertex_stream >> start_vertex_offset_;

    // TODO: of course temporary
    R_ASSERT2( primitive_type == ptTriList
             , "Render doesn't support this topology"
    );
}


/*!
 * \brief   Draws vertex buffer content
 */
void
UiRender::FlushPrimitive()
{
    auto primitives_count = 0;
    std::size_t vertex_offset   = 0;
    std::size_t vertices_count  = 0;

    backend.vertex_stream >> vertex_offset;
    vertex_offset -= start_vertex_offset_;

    // Strides count
    switch (point_type_)
    {
    case pttTL:
        vertices_count = vertex_offset / vertex_format::TriangleList::size;
        break;
    default:
        R_ASSERT(false);
    }
    VERIFY(vertices_count <= vertices_count_);

    // Primitives count
    switch (primitive_type_)
    {
    case ptTriList:
        primitives_count = vertices_count / vertices_per_triangle;
        break;
    default:
        R_ASSERT(false);
    }

    if (primitives_count)
    {
        backend.SetGeometry(backend.vertex_stream);
        backend.Draw(primitives_count);
    }

    primitive_type_ = ptNone;
    point_type_     = pttNone;
}


/*!
 * \brief   Returns new shader name if Theora stream is available
 * \param [in] texture_name normal texture name
 * \param [in] shader_name  default shader name
 */
LPCSTR
UiRender::UpdateShaderName
        ( LPCSTR texture_name
        , LPCSTR shader_name
        )
{
    string_path path;
    const bool is_exist = FS.exist( path
                                  , "$game_textures$"
                                  , texture_name
                                  , ".ogm"
    );

    if (is_exist)
    {
        return ("hud" DELIMITER "movie");
    }
    else
    {
        return shader_name;
    }
}
