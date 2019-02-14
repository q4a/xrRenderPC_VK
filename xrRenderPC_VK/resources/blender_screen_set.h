#ifndef RESOURCES_BLENDER_SCREEN_SET_H_
#define RESOURCES_BLENDER_SCREEN_SET_H_

#include "blender.h"

namespace blenders
{

class ScreenSet
    : public Blender
{
public:
    ScreenSet();
    ~ScreenSet() = default;

    void Load(IReader &rstream, u16 version) final;
    void Save(IWriter &wstream) final;

    LPCSTR getName() final;
    LPCSTR getComment() final;

private:
    xrP_TOKEN   blend_;
    xrP_Integer aref_;
    xrP_BOOL    ztest_;
    xrP_BOOL    zwrite_;
    xrP_BOOL    lighting_;
    xrP_BOOL    fog_;
    xrP_BOOL    clamp_;
};

};

#endif // RESOURCES_BLENDER_SCREEN_SET_H_
