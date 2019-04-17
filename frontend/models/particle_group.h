
#pragma once

#include <memory>

#include "frontend/models/particle_group_descriptor.h"

#include "frontend/models/particle_custom.h"

class ParticleGroup
    : public ParticleCustom
{
    std::shared_ptr<ParticleGroupDescriptor> descriptor_;
public:
    // `IParticleCustom` interface
    void OnDeviceCreate() override {}
    void OnDeviceDestroy() override {}

    void UpdateParent(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM) {}
    void OnFrame(u32 dt) {}

    void Play() {}
    void Stop(BOOL bDefferedStop = TRUE) {}
    BOOL IsPlaying() { return false; }

    u32 ParticlesCount() { return 0; }

    float GetTimeLimit() { return 0.0f; }
    BOOL  IsLooped() { return false; }
    const shared_str Name() { return {}; }
    void SetHudMode(BOOL b) {}
    BOOL GetHudMode() { return false; }

    bool Compile( const std::shared_ptr<ParticleGroupDescriptor> &descriptor
                ) { return true; }
};
