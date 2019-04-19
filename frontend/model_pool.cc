/*!
 * \file    model_pool.cc
 * \brief   Mesh manager implementation
 */

#include "xrCore/FMesh.hpp"
#include "xrGame/GamePersistent.h"

#include "frontend/models/hierrarhy_visual.h"
#include "frontend/models/kinematics_animated.h"
#include "frontend/models/particle_effect.h"
#include "frontend/models/particle_group.h"

#include "frontend/model_pool.h"


//-----------------------------------------------------------------------------
std::unique_ptr<RenderVisual>
ModelPool::CreateInstance
        ( std::uint32_t type
        )
{
    std::unique_ptr<RenderVisual> instance;

    switch (type)
    {
    case MT_HIERRARHY:
        instance = std::make_unique<HierrarhyVisual>();
        break;
    case MT_SKELETON_ANIM:
        instance = std::make_unique<KinematicsAnimated>();
        break;
    case MT_SKELETON_RIGID:
        instance = std::make_unique<Kinematics>();
        break;
    case MT_PARTICLE_EFFECT:
        instance = std::make_unique<ParticleEffect>();
        break;
    case MT_PARTICLE_GROUP:
        instance = std::make_unique<ParticleGroup>();
        break;
    default:
        FATAL("Not implemented");
    }
    instance->type_ = type;
    return instance;
}


//-----------------------------------------------------------------------------
std::shared_ptr<RenderVisual>
ModelPool::DuplicateInstance
        ( const std::shared_ptr<RenderVisual> &source_instance
        )
{
    auto instance = CreateInstance(source_instance->getType());
    instance->Copy(source_instance.get());
    instance->Spawn();
    return instance;
}

//-----------------------------------------------------------------------------
void
ModelPool::RegisterInstance
        ( const std::string                   &name
        , const std::shared_ptr<RenderVisual> &instance
        )
{
    ModelDescription description;
    description.name  = name;
    description.model = instance;
    models_.push_back(description);
}


//-----------------------------------------------------------------------------
std::shared_ptr<RenderVisual>
ModelPool::LoadInstance
        ( const std::string &name
        , bool               need_to_register /* = true */
        )
{
    auto file_name = name + ".ogf";

    string_path path;
    if (!FS.exist(file_name.c_str()))
    {
        if (!FS.exist(path, "$level$", file_name.c_str()))
        {
            if (!FS.exist(path, "$game_meshes$", file_name.c_str()))
            {
                FATAL_F("Can't find model file '%s'.", file_name.c_str());
            }
        }
    }
    else
    {
        xr_strcpy(path, file_name.c_str());
    }

    IReader *rstream = FS.r_open(path);
    ogf_header header;
    rstream->r_chunk_safe( OGF_HEADER
                         , &header
                         , sizeof(header)
    );
    FS.r_close(rstream);

    std::shared_ptr<RenderVisual> instance =
        std::move(CreateInstance(header.type));
    g_pGamePersistent->RegisterModel(instance.get());

    if (need_to_register)
    {
        RegisterInstance(name, instance);
    }

    return instance;
}


//-----------------------------------------------------------------------------
std::shared_ptr<RenderVisual>
ModelPool::FindInstance
        ( const std::string &name
        ) const
{
    for (const auto &model : models_)
    {
        if (model.name == name)
        {
            return model.model;
        }
    }

    return {};
}


//-----------------------------------------------------------------------------
RenderVisual *
ModelPool::CreateModel
        ( const std::string &name
        , IReader           *rstream
        )
{
    VERIFY(name.size());

    // TODO: Cast name to lower case and remove extension
    std::string name_modified = name;

    // Check inactive instances first
    const auto &iterator = inactive_models_.find(name_modified);
    if (iterator != inactive_models_.cend())
    {
        auto &model = iterator->second;
        model->Spawn();
        inactive_models_.erase(iterator);

        return model.get();
    }
    else
    {
        // Check for active instances
        auto instance = FindInstance(name_modified);
        if (!instance)
        {
            VERIFY(rstream == nullptr); // Not expect loading from provided stream
            instance = LoadInstance(name_modified);
        }

        auto model = DuplicateInstance(instance);
        registry_.insert(std::make_pair(model, name_modified));
        return model.get();
    }

    return nullptr;
}


//-----------------------------------------------------------------------------
RenderVisual *
ModelPool::CreateParticleEffect
        ( const std::shared_ptr<ParticleEffectDescriptor> &effect_descriptor
        )
{
    auto instance = CreateInstance(MT_PARTICLE_EFFECT);
    ParticleEffect *pointer =
        dynamic_cast<ParticleEffect *>(instance.get());

    pointer->Compile(effect_descriptor);
    return pointer;
}


//-----------------------------------------------------------------------------
RenderVisual *
ModelPool::CreateParticleGroup
        ( const std::shared_ptr<ParticleGroupDescriptor> &group_descriptor
        )
{
    auto instance = CreateInstance(MT_PARTICLE_GROUP);
    auto pointer =
        dynamic_cast<ParticleGroup *>(instance.release());

    pointer->Compile(group_descriptor);
    return pointer;
}
