#ifndef BACKEND_BACKEND_H_
#define BACKEND_BACKEND_H_

#include <memory>

#include "backend/streams.h"
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
    void Draw(std::uint32_t primitives_count);
    void DrawIndexed(std::uint32_t primitives_count);

    std::shared_ptr<Texture> GetActiveTexture(std::size_t index) const;

    DataStream<VertexStream> vertex_stream;
    DataStream<IndexStream>  index_stream;
    DataStream<IndexStream>  index_cache;

    vk::UniqueRenderPass render_pass;

    std::vector<vk::UniqueSemaphore> frame_semaphores;
    std::vector<vk::UniqueSemaphore> render_semaphores;

private:
    void CreateIndexCache();

    void CreateCommandBuffers();
    void DestroyCommandBuffers();

    void CreateRenderPass();
    void DestroyRenderPass();

    void UpdateDescriptors();
    void BindVertexBuffer(DataStream<VertexStream> &vertices);
    void BindIndexBuffer(DataStream<IndexStream> &indices);

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

        bool scissor = false;
    } state;

    vk::UniqueCommandPool cmd_pool_;
    std::vector<vk::UniqueCommandBuffer> draw_cmd_buffers_;
};

extern BackEnd backend;

#endif // BACKEND_BACKEND_H_
