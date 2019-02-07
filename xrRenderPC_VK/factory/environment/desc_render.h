#ifndef FACTORY_ENVIRONMENT_DESC_RENDER_H_
#define FACTORY_ENVIRONMENT_DESC_RENDER_H_

#include "Include/xrRender/EnvironmentRender.h"

#include "common.h"

class fEnvDescriptorRender : public IEnvDescriptorRender
{
public:
    fEnvDescriptorRender() = default;
    ~fEnvDescriptorRender() = default;

    void Copy(IEnvDescriptorRender& obj) override;
    void OnDeviceCreate(CEnvDescriptor& owner) override;
    void OnDeviceDestroy() override;
};

#endif // FACTORY_ENVIRONMENT_DESC_RENDER_H_
