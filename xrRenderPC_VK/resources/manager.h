#pragma  once

#include <map>
#include <memory>

#include "resources/constant.h"
#include "resources/matrix.h"
#include "resources/blender.h"
#include "resources/texture_desc.h"
#include "resources/shader.h"

class ResourceManager
{
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    void OnDeviceCreate(LPCSTR file_name);
    void OnDeviceDestroy();

    std::shared_ptr<Shader> CreateShader(LPCSTR shader_name,
        LPCSTR textures, LPCSTR constants, LPCSTR matrices);

private:
    std::shared_ptr<Constant> CreateConstant(LPCSTR name);
    void DeleteConstant(const std::shared_ptr<Constant> &constant);

    std::shared_ptr<Matrix> CreateMatrix(LPCSTR name);
    void DeleteMatrix(const std::shared_ptr<Matrix> &matrix);

    std::shared_ptr<Blender> CreateBlender(CLASS_ID class_id);
    std::shared_ptr<Blender> GetBlender(LPCSTR name);

private:
    TextureDescription texture_description_;

    struct str_pred
    {
        bool operator()(LPCSTR x, LPCSTR y) const { return xr_strcmp(x, y) < 0; }
    };

    std::vector<std::shared_ptr<Shader>> shaders_;

    /* Content of `shaders.xr` library file */
    using Constants = std::map<const char *, std::shared_ptr<Constant>>;
    using Matrices  = std::map<const char *, std::shared_ptr<Matrix>>;
    using Blenders  = std::map<const char *, std::shared_ptr<Blender>, str_pred>;

    Constants constants_;
    Matrices  matrices_;
    Blenders  blenders_;
};
