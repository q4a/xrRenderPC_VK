#ifndef RESOURCES_SHADER_PASS_H_
#define RESOURCES_SHADER_PASS_H_

#include <memory>

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

    void MergeConstants(const std::map<std::string, ShaderResource> &);
    void CreatePipelineLayout();
    void AllocateDescriptors();
    void CreatePipeline();

    std::shared_ptr<VertexShader>   vertex_shader;
    std::shared_ptr<FragmentShader> fragment_shader;
    std::map<std::string, ShaderResource> resources;
    vk::PipelineLayout pipeline_layout;
    vk::Pipeline pipeline;
    std::vector<vk::DescriptorSet> descriptors;

    std::map<std::string, std::shared_ptr<Texture>> textures;
    std::map<std::string, SamplerDescription> samplers;

private:
    vk::DescriptorSetLayout descriptors_layout;
};

#endif // RESOURCES_SHADER_PASS_H_
