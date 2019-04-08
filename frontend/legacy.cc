/*!
 * \file    legacy.cc
 * \brief   Unused methods from original frontend interface
 */

#include "frontend/legacy.h"


//-----------------------------------------------------------------------------
void LegacyInterface::SetupGPU
        ( bool force_sw
        , bool force_nonpure
        , bool force_ref
        )
{
    /*
     * Nothing here.
     * We don't support concept of reference devices and
     * expect all TnL processing on HW side.
     */
}


//-----------------------------------------------------------------------------
bool
LegacyInterface::GetForceGPU_REF()
{
    /*
     * Nothing here.
     * We don't support concept of reference devices
     */
    return false;
}


//-----------------------------------------------------------------------------
bool
LegacyInterface::HWSupportsShaderYUV2RGB()
{
    /*
     * Nothing here.
     * The YUV2RGB conversion can be done on CPU side
     * or by shader (if HW can support this). We forced
     * Theora stream decoder to use the second option.
     */

    // TODO: Software coversion is broken due to
    //       use of YUV2RGB shader
    return true;
}


//-----------------------------------------------------------------------------
LPCSTR
LegacyInterface::getShaderPath()
{
    return "r3" DELIMITER ""; // we're using R3 shaders
}
