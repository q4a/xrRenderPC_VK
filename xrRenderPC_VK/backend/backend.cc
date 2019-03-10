#include "backend/backend.h"

constexpr std::size_t kilobyte = 1024;
constexpr std::size_t operator"" _Kb
        ( unsigned long long value
        )
{
    return value * kilobyte;
}

constexpr std::size_t vertex_buffer_size = 4096_Kb;
constexpr std::size_t index_buffer_size  =  512_Kb;

/* Quad index cache */
constexpr std::size_t triangles_per_quad    =    2;
constexpr std::size_t vertices_per_triangle =    3;
constexpr std::size_t triangles_num         = 4096;
constexpr std::size_t index_count           = triangles_num
                                              * triangles_per_quad
                                              * vertices_per_triangle;

constexpr std::size_t index_cache_size      = index_count * Index::size;


/**
 *
 */
BackEnd::BackEnd()
    : vertex_stream_(vertex_buffer_size)
    , index_stream_(index_buffer_size)
    , index_cache_(index_cache_size)
{
}


/**
 *
 */
void
BackEnd::OnDeviceCreate()
{
    CreateIndexCache();

    vertex_stream_.Create();
    index_stream_.Create();
}


/**
 *
 */
void
BackEnd::OnDeviceDestroy()
{

}


/**
 *
 */
void
BackEnd::CreateIndexCache()
{
    index_cache_.Create();

    for ( auto triangle_idx = 0
        ; triangle_idx < triangles_num
        ; triangle_idx++
        )
    {
        const auto start_vertex =
            triangle_idx * (vertices_per_triangle + 1);

        index_cache_ << Index{ std::uint16_t(start_vertex + 0) };
        index_cache_ << Index{ std::uint16_t(start_vertex + 1) };
        index_cache_ << Index{ std::uint16_t(start_vertex + 2) };

        index_cache_ << Index{ std::uint16_t(start_vertex + 3) };
        index_cache_ << Index{ std::uint16_t(start_vertex + 2) };
        index_cache_ << Index{ std::uint16_t(start_vertex + 0) };
    }

    index_cache_ << StreamControl::Flush;
}
