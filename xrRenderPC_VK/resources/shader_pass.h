#ifndef RESOURCES_SHADER_PASS_H_
#define RESOURCES_SHADER_PASS_H_

#include "resources/pipeline_shader.h"

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
    vk::Pipeline pipeline;

private:
    vk::DescriptorSetLayout descriptors_layout;
    vk::PipelineLayout pipeline_layout;
    std::vector<vk::DescriptorSet> descriptors;
};

#endif // RESOURCES_SHADER_PASS_H_
