#ifndef RESOURCES_TEXTURE_H_
#define RESOURCES_TEXTURE_H_

#include <functional>
#include <memory>
#include <string>

#include "backend/streams.h"

class Texture
    : public xr_resource_named
{
    enum class TextureType
    {
        Unknown,
        Normal,
        Theora,
        Avi,
        Sequence
    };

    TextureType type = TextureType::Unknown;

    struct Flags
    {
        bool user    = false;
        bool loaded  = false; ///< Indicates that texture is already loaded
        bool staging = false; ///< Defer image uploading until pipeline binding
    } flags;

    void TextureLoad(const std::string &file_name);
    void ApplyLoad();

    void ApplyTheora();
    void ApplyAvi();
    void ApplySequence();
    void ApplyNormal();

 
public:
    Texture();

    void Preload();
    void Load();
    void Postload();

    std::function<void(void)> Loader;

    std::shared_ptr<StreamImage> image;
};

#endif // RESOURCES_TEXTURE_H_
