#ifndef RESOURCES_TEXTURE_DESC_H_
#define RESOURCES_TEXTURE_DESC_H_

class TextureDescription
{
public:
    TextureDescription() = default;
    ~TextureDescription() = default;

    void Load();
    void Unload();
};

#endif // RESOURCES_TEXTURE_DESC_H_
