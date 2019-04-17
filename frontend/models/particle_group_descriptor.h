/*!
 * \file    particle_group_descriptor.h
 *
 * Particle group resource descriptor interface
 */

#ifndef FRONTEND_MODELS_PARTICLE_GROUP_DESCRIPTOR_H_
#define FRONTEND_MODELS_PARTICLE_GROUP_DESCRIPTOR_H_

#include <string>


class ParticleGroupDescriptor
{
    std::string name_;
public:
    ParticleGroupDescriptor() = default;
    ~ParticleGroupDescriptor() = default;

    /*!
     * \brief   Initializes object from serialized data
     */
    bool Load( IReader &stream ///< stream to read from
             );
    /*!
     * \brief   Returns a name of the resource
     */
    const std::string &GetName() const;
};

#endif // FRONTEND_MODELS_PARTICLE_GROUP_DESCRIPTOR_H_
