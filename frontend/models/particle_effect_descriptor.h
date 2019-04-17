/*!
 * \file    particle_effect_descriptor.h
 *
 * Particle group resource descriptor interface
 */

#ifndef FRONTEND_MODELS_PARTICLE_EFFECT_DESCRIPTOR_H_
#define FRONTEND_MODELS_PARTICLE_EFFECT_DESCRIPTOR_H_

#include <string>


class ParticleEffectDescriptor
{
    std::string name_;
public:
    ParticleEffectDescriptor() = default;
    ~ParticleEffectDescriptor() = default;

    /*!
     * \brief   Initializes object from serialized data
     */
    bool Load( IReader &stream ///< stream to read from
             );
    /*!
     * \brief   Returns a name of the resource
     */
    const std::string &GetName() const;

    /*!
     * \brief   Creates effect's shader
     */
    void CreateShader();
};

#endif // FRONTEND_MODELS_PARTICLE_EFFECT_DESCRIPTOR_H_
