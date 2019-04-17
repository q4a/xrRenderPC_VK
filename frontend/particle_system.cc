/*
 * \file    particle_system.h
 * \brief   Particle system library interface
 */

#include "frontend/particle_system.h"

constexpr char         *library__name = "particles.xr";
constexpr std::uint16_t library__supported_version = 1;

enum class LibraryChunks : std::uint32_t
{
    Version = 1,
    Effects = 3,
    Groups  = 4
};


/*!
 * \brief   Converts `LibraryChunks` element into integer
 */
static auto
ChunkId
        ( LibraryChunks chunk ///< enumeration type
        )
{
    return static_cast<std::uint32_t>(chunk);
}


//-----------------------------------------------------------------------------
void
ParticleSystem::OnCreate()
{
    string_path path;
    FS.update_path( path
                  , _game_data_
                  , library__name
    );

    Load(path);
}


//-----------------------------------------------------------------------------
void
ParticleSystem::OnDestroy()
{
}


//-----------------------------------------------------------------------------
void
ParticleSystem::Load
        ( const std::string &path_to_library
        )
{
    VERIFY(FS.exist(path_to_library.c_str()));

    Msg("* Loading '%s'", path_to_library.c_str());

    auto reader = FS.r_open(path_to_library.c_str());
    VERIFY(reader->find_chunk(ChunkId(LibraryChunks::Version)));

    auto version = reader->r_u16();
    VERIFY(version == library__supported_version);

    // Load effects
    if  (reader->find_chunk(ChunkId(LibraryChunks::Effects)))
    {
        auto effects_reader =
            reader->open_chunk(ChunkId(LibraryChunks::Effects));

        std::size_t offset = 0;

        while (effects_reader->find_chunk(offset))
        {
            auto descriptor = std::make_shared<ParticleEffectDescriptor>();
            auto stream = effects_reader->open_chunk(offset);

            VERIFY(descriptor->Load(*stream));
            stream->close();

            effects_.insert(std::make_pair( descriptor->GetName()
                                          , descriptor
            ));

            offset++;
        }
        effects_reader->close();
    }

    // Load groups
    if (reader->find_chunk(ChunkId(LibraryChunks::Groups)))
    {
        auto groups_reader =
            reader->open_chunk(ChunkId(LibraryChunks::Groups));

        std::size_t offset = 0;

        while (groups_reader->find_chunk(offset))
        {
            auto descriptor = std::make_shared<ParticleGroupDescriptor>();
            auto stream = groups_reader->open_chunk(offset);

            VERIFY(descriptor->Load(*stream));
            stream->close();

            groups_.insert(std::make_pair( descriptor->GetName()
                                         , descriptor
            ));

            offset++;
        }
        groups_reader->close();
    }

    FS.r_close(reader);

    // Create shaders
    for (const auto &[name, effect] : effects_)
    {
        effect->CreateShader();
    }
}


//-----------------------------------------------------------------------------
std::shared_ptr<ParticleEffectDescriptor>
ParticleSystem::FindParticleEffectDescriptor
        ( const std::string &effect_name
        )
{
    const auto &iterator = effects_.find(effect_name);
    
    if (iterator != effects_.cend())
    {
        return iterator->second;
    }

    return {};
}


//-----------------------------------------------------------------------------
std::shared_ptr<ParticleGroupDescriptor>
ParticleSystem::FindParticleGroupDescriptor
        ( const std::string &group_name
        )
{
    const auto &iterator = groups_.find(group_name);

    if (iterator != groups_.cend())
    {
        return iterator->second;
    }

    return {};
}
