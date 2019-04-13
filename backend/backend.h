#ifndef BACKEND_BACKEND_H_
#define BACKEND_BACKEND_H_

#include <array>
#include <memory>

#include "backend/streams.h"
#include "frontend/render_target.h"
#include "resources/shader.h"

constexpr std::size_t triangles_per_quad    = 2;
constexpr std::size_t vertices_per_triangle = 3;

class BackEnd
{
public:
    BackEnd();
    ~BackEnd() = default;

    void OnDeviceCreate();
    void OnDeviceDestroy();

    void OnFrameBegin(std::uint32_t frame_index);
    void OnFrameEnd(std::uint32_t frame_index);

    /* Render state control */
    void SetClearColor(const std::array<float, 4> &clear_color);
    void SetRenderPass( const PassDescription &pass_description
                      );

    /*!
     * \brief   Sets scissor test parameters for current pass
     */
    void SetScissor( const vk::Rect2D &scissor ///< [in] scissor rectangle area
                   , bool              enable  ///< [in] do scissor test
                   );
    void SetShader(const std::shared_ptr<Shader> &shader);
    void SetGeometry( DataStream<VertexStream> &vertices
                    , DataStream<IndexStream>  &indices
                    );
    void SetGeometry( DataStream<VertexStream> &vertices );
    /*!
    * \brief   Draws triangles list
    *
    * TODO: Need to support TS, lines and other topologies
    */
    void Draw( std::uint32_t primitives_count ///< [in] amount of triangles to draw
             );
    /*!
     * \brief   Draws indexed triangles list
     *
     * TODO: Need to support TS, lines and other topologies
     */
    void DrawIndexed( std::uint32_t primitives_count ///< [in] amount of triangles to draw
                    );

    std::shared_ptr<Texture> GetActiveTexture(std::size_t index) const;

    DataStream<VertexStream> vertex_stream;
    DataStream<IndexStream>  index_stream;
    DataStream<IndexStream>  index_cache;

    std::vector<vk::UniqueSemaphore> frame_semaphores;
    std::vector<vk::UniqueSemaphore> render_semaphores;

private:
    void CreateIndexCache();

    void CreateCommandBuffers();
    void DestroyCommandBuffers();

    /*!
     * \brief   Updates descriptors for current pipeline
     */
    void UpdateDescriptors();
    /*!
     * \brief   Binds vertex buffer to pipeline
     *
     * Flushes vertex buffer before binding and binds it starting from offset
     * set by previous flush. The function updates render state.
     */
    void BindVertexBuffer( DataStream<VertexStream> &vertices ///< [in] vertex stream
                         );
    /*!
     * \brief   Binds index buffer to pipeline
     *
     * Flushes index buffer before binding and binds it starting from offset
     * set by previous flush. The function updates render state.
     */
    void BindIndexBuffer( DataStream<IndexStream> &indices ///< [in] index stream
                        );
    /*!
     * \brief   Invalidates render state
     */
    void InvalidateState();

    struct RenderState
    {
        std::shared_ptr<ShaderPass> pass;
        DataStream<VertexStream> *vertices; // TODO: not quite elegant
        DataStream<IndexStream>  *indices;
        std::uint32_t frame_index;

        // Render pass state
        std::array<float, 4> clear_color;
        vk::RenderPass render_pass;

        bool scissor = false;
    } state;

    vk::UniqueCommandPool cmd_pool_;
    std::vector<vk::UniqueCommandBuffer> draw_cmd_buffers_;
};

extern BackEnd backend;

#endif // BACKEND_BACKEND_H_
