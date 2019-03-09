#ifndef RESOURCES_MANAGER_H_
#define RESOURCES_MANAGER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "backend/streams.h"
#include "resources/constant.h"
#include "resources/matrix.h"
#include "resources/blender.h"
#include "resources/texture_desc.h"
#include "resources/shader.h"
#include "resources/shader_element.h"
#include "resources/shader_pass.h"
#include "resources/pipeline_shader.h"
#include "resources/texture.h"


using VertexShaderPtr   = std::shared_ptr<VertexShader>;
using FragmentShaderPtr = std::shared_ptr<FragmentShader>;

using VertexShaders   = std::map<std::string, VertexShaderPtr>;
using FragmentShaders = std::map<std::string, FragmentShaderPtr>;


class ResourceManager
{
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    void OnDeviceCreate(const std::string &file_name);
    void OnDeviceDestroy();

    std::shared_ptr<Shader> CreateShader(const std::string &shader_name,
        const std::string &textures, const std::string &constants,
        const std::string &matrices);

    std::shared_ptr<ShaderElement> CreateShaderElement(const ShaderElement &element);
    std::shared_ptr<ShaderPass> CreateShaderPass(const ShaderPass &pass);

    std::shared_ptr<Texture> CreateTexture(const std::string &name);

    VertexShaderPtr CreateVertexShader(const std::string &name);
    FragmentShaderPtr CreateFragmentShader(const std::string &name);

private:
    std::shared_ptr<Constant> CreateConstant(LPCSTR name);
    void DeleteConstant(const std::shared_ptr<Constant> &constant);

    std::shared_ptr<Matrix> CreateMatrix(LPCSTR name);
    void DeleteMatrix(const std::shared_ptr<Matrix> &matrix);

    std::shared_ptr<Blender> CreateBlender(CLASS_ID class_id);
    std::shared_ptr<Blender> GetBlender(const std::string &name);

    void ParseList(const std::string &list, std::vector<std::string> &strings);

    template <class T>
    std::map<std::string, std::shared_ptr<T>> &GetShaderMap();

    template <class T>
    std::shared_ptr<T> CreatePipelineShader(const std::string &name,
        const std::string &file_name);

    bool CompileShader(const std::string &name, PipelineShader *shader);

private:
    TextureDescription texture_description_;

    VertexShaders   vertex_shaders_;
    FragmentShaders fragment_shaders_;

    std::map<std::string, std::string> shader_options_;

    using Shaders        = std::vector<std::shared_ptr<Shader>>;
    using ShaderElements = std::vector<std::shared_ptr<ShaderElement>>;
    using ShaderPasses   = std::vector<std::shared_ptr<ShaderPass>>;

    Shaders        shaders_;
    ShaderElements elements_;
    ShaderPasses   shader_passes_;

    /* Content of `shaders.xr` library file */
    using Constants = std::map<std::string, std::shared_ptr<Constant>>;
    using Matrices  = std::map<std::string, std::shared_ptr<Matrix>>;
    using Blenders  = std::map<std::string, std::shared_ptr<Blender>>;

    Constants constants_;
    Matrices  matrices_;
    Blenders  blenders_;
};

#endif // RESOURCES_MANAGER_H_
