#ifndef RESOURCES_BLENDER_COMPILER_H_
#define RESOURCES_BLENDER_COMPILER_H_

#include <array>
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <utility>

#include "resources/constant_table.h"
#include "resources/blender.h"
#include "resources/pipeline_shader.h"
#include "resources/shader.h"
#include "resources/shader_element.h"
#include "resources/shader_pass.h"
#include "resources/texture.h"

constexpr std::size_t max_shader_stages = 2; ///< Vertex, Fragment

class BlenderCompiler
{
public:
    std::shared_ptr<ShaderElement> Compile();
    std::shared_ptr<ShaderElement>
    CompileLua( const std::string &name_space
              , const std::string &shader_name
              );

    void PassBegin( const std::string &vertex_shader
                  , const std::string &geometry_shader
                  , const std::string &fragment_shader
                  );
    void PassTexture(const std::string &name, const std::string &texture);
    void PassSampler(const std::string &name);
    void PassZtest(bool z_test, bool z_write, bool invert_test = false);
    void PassBlend(bool alpha_blend, vk::BlendFactor blend_source, vk::BlendFactor blend_destination,
        bool alpha_test, u32 alpha_reference);
    void PassLightFog(bool lighting, bool fog) {}
    void PassEnd();

    // Blending control
    void BlendAlphaMode(bool, vk::BlendFactor, vk::BlendFactor);
    void BlendAlphaReference(bool alpha_test, std::uint32_t alpha_reference);
    void BlendColorWriteMask(bool out_r, bool out_g, bool out_b, bool out_a);

    // Sampler control
    void SamplerAnisotropy(const std::string &sampler_name, bool enable);
    void SamplerAddressing(const std::string &sampler_name, vk::SamplerAddressMode mode);
    void SamplerFiltering( const std::string    &sampler_name
                         , vk::Filter            magnifier
                         , vk::Filter            minifier
                         , vk::SamplerMipmapMode mip_mode
                         );

    std::vector<std::string> textures; ///< binded textures names
    std::string detail_texture; ///< detail texture name
    std::function<void()> detail_scaler; ///< detail scaler

    std::vector<std::string> constants; // TODO: seems deprecated
    std::vector<std::string> matrices; // TODO: seems deprecated

    std::shared_ptr<ShaderElement> shader_element; ///< Compilation target
    ShaderElementType current_element; ///< Compilation target stage
    ShaderPass pass; ///< Intermediate pass data

    std::shared_ptr<Blender> blender;

    bool detail   = false;
    bool emissive = false;
    bool distort  = false;
    bool wallmark = false;

private:
    void SetDefaultBindings(ConstantTable &table) const;
};

#endif // RESOURCES_BLENDER_COMPILER_H_
