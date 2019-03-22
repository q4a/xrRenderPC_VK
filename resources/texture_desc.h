#ifndef RESOURCES_TEXTURE_DESC_H_
#define RESOURCES_TEXTURE_DESC_H_

#include <functional>
#include <string>

class TextureDescription
{
public:
    TextureDescription() = default;
    ~TextureDescription() = default;

    void Load();
    void Unload();

    /*!
     * \brief   Checks if the texture can be detailed
     */
    bool GetDetailTexture( const std::string     &texture_name  ///< [in]  texture name to check
                         , std::string           &detail_name   ///< [out] detail texture name
                         , std::function<void()> &detail_scaler ///< [out] scaler function
                         );

private:
    void LoadThm(const std::string &mount_point);
};

#endif // RESOURCES_TEXTURE_DESC_H_
