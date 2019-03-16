#ifndef RESOURCES_BLENDER_COMPILER_H_
#define RESOURCES_BLENDER_COMPILER_H_

#include <array>
#include <memory>
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
    void Compile(ShaderElement &element);
    void PassBegin(const std::string &vertex_shader, const std::string &fragment_shader);
    void PassTexture(const std::string &name, const std::string &texture);
    void PassSampler(const std::string &name);
    void PassZtest(bool z_test, bool z_write, bool invert_test = false);
    void PassBlend(bool alpha_blend, vk::BlendFactor blend_source, vk::BlendFactor blend_destination,
        bool alpha_test, u32 alpha_reference);
    void PassAlphaBlendMode(bool, vk::BlendFactor, vk::BlendFactor);
    void PassAlphaBlendKey(bool, u32);
    void PassLightingFog(bool lighting, bool fog) {}
    void PassEnd();

    // Sampler control
    void SamplerAddressing(const std::string &name, vk::SamplerAddressMode mode);

    std::vector<std::string> textures;
    std::vector<std::string> constants; // TODO: seems deprecated
    std::vector<std::string> matrices; // TODO: seems deprecated

    ShaderElement *shader_element; ///< Compilation target
    ShaderElementType current_element; ///< Compilation target stage
    ShaderPass pass; ///< Intermediate pass data

    std::shared_ptr<Blender> blender;
    bool detail = false;
private:
    void SetMapping();
};

#endif // RESOURCES_BLENDER_COMPILER_H_
