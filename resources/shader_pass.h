#ifndef RESOURCES_SHADER_PASS_H_
#define RESOURCES_SHADER_PASS_H_

#include <memory>
#include <map>

#include "resources/pipeline_shader.h"
#include "resources/texture.h"

struct SamplerDescription
{
    vk::SamplerCreateInfo create_info;
    vk::Sampler sampler;
};

class ShaderPass
{
public:
    bool IsEqual(const ShaderPass &pass) const;

    void MergeResources(const std::shared_ptr<PipelineShader> &shader);
    void CreatePipelineLayout();
    void AllocateDescriptors();
    void CreatePipeline();

    std::shared_ptr<VertexShader>   vertex_shader;
    std::shared_ptr<FragmentShader> fragment_shader;
    vk::PipelineLayout pipeline_layout;
    std::map<vk::RenderPass, vk::Pipeline> pipelines;
    std::vector<vk::DescriptorSet> descriptors;

    std::map<std::string, std::shared_ptr<ShaderResource>> resources; ///< resources gathered from shaders stages

    std::map<std::string, std::shared_ptr<ConstantTable>> constants; ///< shader uniforms
    std::map<std::string, std::shared_ptr<Texture>>       textures; ///< textures requested by blender
    std::map<std::string, SamplerDescription>             samplers; ///< samplers requested by blender

    std::uint32_t frame_when_updated = 0;

private:
    vk::DescriptorSetLayout descriptors_layout;
};

#endif // RESOURCES_SHADER_PASS_H_
