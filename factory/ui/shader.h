#ifndef FACTORY_UI_SHADER_H_
#define FACTORY_UI_SHADER_H_

#include <memory>

#include "Include/xrRender/UIShader.h"

#include "resources/shader.h"

class fUIShader : public IUIShader
{
    friend class UiRender;

    std::shared_ptr<Shader> shader_;
public:
    fUIShader() = default;
    ~fUIShader() = default;

    // Interface implementation
    void Copy(IUIShader& obj) override;
    void create(LPCSTR shaderName, LPCSTR textureList = nullptr) override;
    bool inited() override;
    void destroy() override;
};

#endif // FACTORY_UI_SHADER_H_
