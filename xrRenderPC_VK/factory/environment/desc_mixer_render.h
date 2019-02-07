#ifndef FACTORY_ENVIRONMENT_DESC_MIXER_RENDER_H_
#define FACTORY_ENVIRONMENT_DESC_MIXER_RENDER_H_

#include "common.h"

#include "Include/xrRender/EnvironmentRender.h"


class fEnvDescriptorMixerRender : public IEnvDescriptorMixerRender
{
public:
    fEnvDescriptorMixerRender() = default;
    ~fEnvDescriptorMixerRender() = default;

    void Copy(IEnvDescriptorMixerRender& obj) override;
    void Destroy() override;
    void Clear() override;
    void lerp(IEnvDescriptorRender *in_a, IEnvDescriptorRender *in_b) override;
};

#endif // FACTORY_ENVIRONMENT_DESC_MIXER_RENDER_H_
