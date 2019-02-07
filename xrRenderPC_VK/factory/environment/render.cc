#include "factory/environment/render.h"


/**
 *
 */
void
fEnvironmentRender::Copy
        ( IEnvironmentRender& obj
        )
{
    *this = *((fEnvironmentRender *)&obj);
}


/**
 *
 */
void
fEnvironmentRender::OnFrame
        ( CEnvironment& env
        )
{
    // TBI
}


/**
 *
 */
void
fEnvironmentRender::OnLoad()
{
    // TBI
}


/**
 *
 */
void
fEnvironmentRender::OnUnload()
{
    // TBI
}


/**
 *
 */
void
fEnvironmentRender::RenderSky
        ( CEnvironment& env
        )
{
    // TBI
}


/**
 *
 */
void
fEnvironmentRender::RenderClouds
        ( CEnvironment& env
        )
{
    // TBI
}


/**
 *
 */
void
fEnvironmentRender::OnDeviceCreate()
{
    // TBI
}


/**
 *
 */
void
fEnvironmentRender::OnDeviceDestroy()
{
    // TBI
}


// Dummy implementation
class Dummy : public particles_systems::library_interface
{
public:
    PS::CPGDef const* const* particles_group_begin() const { return nullptr; };
    PS::CPGDef const* const* particles_group_end() const { return nullptr; }
    void particles_group_next(PS::CPGDef const* const*& iterator) const {};
    shared_str const& particles_group_id(PS::CPGDef const& particles_group) const { return "nothing"; };
} dummy;


/**
 *
 */
particles_systems::library_interface const&
fEnvironmentRender::particles_systems_library()
{
    // TBI
    return dummy;
}
