#ifndef FACTORY_FONT_RENDER_H_
#define FACTORY_FONT_RENDER_H_

#include "Include/xrRender/FontRender.h"

#include "common.h"

class fFontRender : public IFontRender
{
public:
    fFontRender() = default;
    ~fFontRender() = default;

    void Initialize(LPCSTR shader, LPCSTR texture) override;
    void OnRender(CGameFont& owner) override;
};

#endif // FACTORY_FONT_RENDER_H_
