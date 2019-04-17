
#include "frontend/models/particle_group_descriptor.h"

constexpr std::uint16_t group__supported_version = 3;

enum class GroupChunks : std::uint32_t
{
    Version = 1,
    Name = 2,

};

/*!
 * \brief   Converts `EffectChunks` element into integer
 */
static auto
ChunkId
        ( GroupChunks chunk ///< enumeration type
        )
{
    return static_cast<std::uint32_t>(chunk);
}


//-----------------------------------------------------------------------------
bool
ParticleGroupDescriptor::Load
        ( IReader &stream
        )
{
    // Version
    VERIFY(stream.find_chunk(ChunkId(GroupChunks::Version)));
    auto version = stream.r_u16();
    if (version != group__supported_version)
    {
        Msg("! Unsupported particle effect version");
        return false;
    }

    // Name
    xr_string name;
    VERIFY(stream.find_chunk(ChunkId(GroupChunks::Name)));
    stream.r_stringZ(name);
    name_ = name;

    return true;
}


//-----------------------------------------------------------------------------
const std::string &
ParticleGroupDescriptor::GetName() const
{
    return name_;
}
