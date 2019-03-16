#ifndef RESOURCES_MATRIX_H_
#define RESOURCES_MATRIX_H_

#include "xrCore/xr_resource.h"
#include "xrEngine/WaveForm.h"

class Matrix
    : public xr_resource_named
{
public:
    void Load(IReader *rstream);

private:
    u32 mode_;
    u32 tcm_;
    WaveForm scale_u_;
    WaveForm scale_v_;
    WaveForm rotate_;
    WaveForm scroll_u_;
    WaveForm scroll_v_;
};

#endif // RESOURCES_MATRIX_H_
