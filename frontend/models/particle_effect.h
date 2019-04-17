#pragma once

#include <memory>

#include "frontend/models/particle_effect_descriptor.h"

#include "frontend/models/particle_custom.h"

class ParticleEffect final
    : public ParticleCustom
{
public:
    // `IParticleCustom` interface
    void OnDeviceCreate() override {}
    void OnDeviceDestroy() override {}

    void UpdateParent(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM) {}
    void OnFrame(u32 dt) {}
    void Play() {}
    void Stop(BOOL bDefferedStop = TRUE) {}
    BOOL IsPlaying() { return false; }
    void SetHudMode(BOOL b) {}
    BOOL GetHudMode() { return false; }
    u32 ParticlesCount() { return 0; }
    const shared_str Name() { return {}; }
    float GetTimeLimit() { return 0.0f; }

    bool Compile( const std::shared_ptr<ParticleEffectDescriptor> &descriptor
                ) { return true; }
};
