/*!
 * \file    model_pool.h
 * \brief   Mesh manager interface
 */

#ifndef FRONTEND_MODEL_POOL_H_
#define FRONTEND_MODEL_POOL_H_

#include <map>
#include <memory>
#include <vector>

#include "frontend/models/particle_effect_descriptor.h"
#include "frontend/models/particle_group_descriptor.h"
#include "frontend/models/render_visual.h"


class ModelPool
{
    struct ModelDescription
    {
        std::string name;
        std::shared_ptr<RenderVisual> model;
    };

    std::shared_ptr<RenderVisual> FindInstance( const std::string &name
                                              ) const;
    std::shared_ptr<RenderVisual> LoadInstance( const std::string &name
                                              , bool               need_to_register = true
                                              );
    std::shared_ptr<RenderVisual> DuplicateInstance( const std::shared_ptr<RenderVisual> &instance
                                                   );
    void RegisterInstance( const std::string                   &name
                         , const std::shared_ptr<RenderVisual> &instance
                         );
    std::unique_ptr<RenderVisual> CreateInstance( std::uint32_t type
                                                );

    std::vector<ModelDescription> models_;
    std::multimap<std::string, std::shared_ptr<RenderVisual>> inactive_models_;
    std::map<std::shared_ptr<RenderVisual>, std::string> registry_;
public:
    ModelPool() = default;
    ~ModelPool() = default;

    /*!
     * \brief   Creates model object
     */
    RenderVisual *CreateModel( const std::string &name ///< model asset name
                             , IReader           *rstream ///< stream to read from
                             );
    RenderVisual *CreateParticleEffect(const std::shared_ptr<ParticleEffectDescriptor> &effect_descriptor);
    RenderVisual *CreateParticleGroup(const std::shared_ptr<ParticleGroupDescriptor> &group_descriptor);
};

#endif // FRONTEND_MODEL_POOL_H_
