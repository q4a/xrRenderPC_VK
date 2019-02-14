#pragma  once

#include <map>
#include <memory>

#include "resources/constant.h"
#include "resources/matrix.h"
#include "resources/blender.h"
#include "resources/texture_desc.h"

class ResourceManager
{
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    void OnDeviceCreate(LPCSTR file_name);
    void OnDeviceDestroy();

private:
    std::shared_ptr<Constant> CreateConstant(LPCSTR name);
    void DeleteConstant(const std::shared_ptr<Constant> &constant);

    std::shared_ptr<Matrix> CreateMatrix(LPCSTR name);
    void DeleteMatrix(const std::shared_ptr<Matrix> &matrix);

    std::shared_ptr<Blender> CreateBlender(CLASS_ID class_id);
    std::shared_ptr<Blender> GetBlender(LPCSTR name);


private:
    TextureDescription texture_description_;

    /* Content of `shaders.xr` library file */
    using Constants = std::map<const char*, std::shared_ptr<Constant>>;
    using Matrices  = std::map<const char*, std::shared_ptr<Matrix>>;
    using Blenders  = std::map<const char*, std::shared_ptr<Blender>>;

    Constants constants_;
    Matrices  matrices_;
    Blenders  blenders_;
};
