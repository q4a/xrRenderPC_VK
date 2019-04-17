
#include "frontend/models/kinematics_animated.h"


//-----------------------------------------------------------------------------
IRenderVisual *
KinematicsAnimated::dcast_RenderVisual()
{
    return this;
}


//-----------------------------------------------------------------------------
IKinematics *
KinematicsAnimated::dcast_PKinematics()
{
    return this;
}
