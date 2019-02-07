#ifndef FACTORY_THUNDERBOLT_DESC_RENDER_H_
#define FACTORY_THUNDERBOLT_DESC_RENDER_H_

#include "Include/xrRender/ThunderboltDescRender.h"

#include "common.h"

class fThunderboltDescRender : public IThunderboltDescRender
{
public:
    fThunderboltDescRender() = default;
    ~fThunderboltDescRender() = default;

    void Copy(IThunderboltDescRender& obj) override;
    void CreateModel(LPCSTR modelName) override;
    void DestroyModel() override;
};

#endif // FACTORY_THUNDERBOLT_DESC_RENDER_H_
