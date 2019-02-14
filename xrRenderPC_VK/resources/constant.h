#ifndef RESOURCES_CONSTANT_H_
#define RESOURCES_CONSTANT_H_

#include "xrCore/xr_resource.h"
#include "xrEngine/WaveForm.h"

class Constant
    : public xr_resource_named
{
public:
    void Load(IReader *rstream);

private:
    WaveForm r_;
    WaveForm g_;
    WaveForm b_;
    WaveForm a_;
};

#endif // RESOURCES_CONSTANT_H_
