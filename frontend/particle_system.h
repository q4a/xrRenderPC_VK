/*
 * \file    particle_system.h
 * \brief   Particle system library interface
 */

#ifndef FRONTEND_PARTICLE_SYSTEM_H_
#define FRONTENT_PARTICLE_SYSTEM_H_

class ParticleSystem
{
public:
    ParticleSystem() = default;
    ~ParticleSystem() = default;

    void OnCreate();
    void OnDestroy();
};

#endif // FRONTEND_PARTICLE_SYSTEM_H_
