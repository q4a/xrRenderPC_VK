/*
 * \file    particle_system.h
 * \brief   Particle system library interface
 */

#ifndef FRONTEND_PARTICLE_SYSTEM_H_
#define FRONTENT_PARTICLE_SYSTEM_H_

#include <map>
#include <memory>
#include <string>

#include "Include/xrRender/particles_systems_library_interface.hpp"

#include "models/particle_effect_descriptor.h"
#include "models/particle_group_descriptor.h"


class ParticleSystem final
    //: public particles_systems::library_interface
{
    /*!
     * \brief   Loads particles library
     *
     * NOTE: This functions expects no I/O fail
     */
    void Load( const std::string &path ///< path to particles library file
             );

    std::map<std::string, std::shared_ptr<ParticleEffectDescriptor>> effects_;
    std::map<std::string, std::shared_ptr<ParticleGroupDescriptor>>  groups_;
public:
    ParticleSystem() = default;
    ~ParticleSystem() = default;

    void OnCreate();
    void OnDestroy();

    std::shared_ptr<ParticleEffectDescriptor> FindParticleEffectDescriptor(const std::string &effect_name);
    std::shared_ptr<ParticleGroupDescriptor>  FindParticleGroupDescriptor(const std::string &group_name);
};

#endif // FRONTEND_PARTICLE_SYSTEM_H_
