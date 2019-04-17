
#include "frontend/models/particle_effect_descriptor.h"

constexpr std::uint16_t effect__supported_version = 1;

enum class EffectChunks : std::uint32_t
{
    Version = 1,
    Name = 2,

};

/*!
 * \brief   Converts `EffectChunks` element into integer
 */
static auto
ChunkId
        ( EffectChunks chunk ///< enumeration type
        )
{
    return static_cast<std::uint32_t>(chunk);
}


//-----------------------------------------------------------------------------
bool
ParticleEffectDescriptor::Load
        ( IReader &stream
        )
{
    // Version
    VERIFY(stream.find_chunk(ChunkId(EffectChunks::Version)));
    auto version = stream.r_u16();
    if (version != effect__supported_version)
    {
        Msg("! Unsupported effect version");
        return false;
    }

    // Name
    xr_string name;
    VERIFY(stream.find_chunk(ChunkId(EffectChunks::Name)));
    stream.r_stringZ(name);
    name_ = name;

    return true;
}


//-----------------------------------------------------------------------------
const std::string &
ParticleEffectDescriptor::GetName() const
{
    return name_;
}


//-----------------------------------------------------------------------------
void
ParticleEffectDescriptor::CreateShader()
{
}
