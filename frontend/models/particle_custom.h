/*!
 * \file    particle_custom.h
 */

#ifndef FRONTEND_MODELS_PARTICLE_CUSTOM_H_
#define FRONTEND_MODELS_PARTICLE_CUSTOM_H_

#include "Include/xrRender/ParticleCustom.h"

#include "frontend/models/render_visual.h"


class ParticleCustom
    : public RenderVisual
    , public IParticleCustom
{
public:
    IParticleCustom *dcast_ParticleCustom() final
    {
        return this;
    }
};

#endif // FRONTEND_MODELS_PARTICLE_CUSTOM_H_
