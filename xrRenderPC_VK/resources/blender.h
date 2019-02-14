#ifndef RESOURCES_BLENDER_H_
#define RESOURCES_BLENDER_H_

#include <memory>

#include "xrEngine/Properties.h"


class BlenderDescription
{
public:
    CLASS_ID class_id;
    string128 name;
    string32 computer_name;
    u32 time;
    u16 version;
};


class Blender
    : public CPropertyBase
{
public:
    Blender() = default;
    virtual ~Blender() {};

    BlenderDescription &GetDescription();
    void Load(IReader &rstream, u16 version) override;
    void Save(IWriter &wstream) override;

    LPCSTR getName() override;
    LPCSTR getComment() = 0;

protected:
    BlenderDescription  description;
    xrP_Integer         priority;
    xrP_BOOL            strict_sorting;
    string64            name;
    string64            xform;
};

#endif // RESOURCES_BLENDER_H_
