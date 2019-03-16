#ifndef FACTORY_FONT_RENDER_H_
#define FACTORY_FONT_RENDER_H_

#include <memory>

#include "Include/xrRender/FontRender.h"

#include "resources/shader.h"

class fFontRender : public IFontRender
{
    std::shared_ptr<Shader> shader_;
public:
    fFontRender() = default;
    ~fFontRender() = default;

    void Initialize(LPCSTR shader_name, LPCSTR texture_name) final;
    void OnRender(CGameFont& owner) final;
};

#endif // FACTORY_FONT_RENDER_H_
