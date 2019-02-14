#include "resources/matrix.h"


/**
 *
 */
void
Matrix::Load
        ( IReader *rstream
        )
{
    mode_ = rstream->r_u32();
    tcm_  = rstream->r_u32();
    rstream->r(&scale_u_, sizeof (scale_u_));
    rstream->r(&scale_v_, sizeof (scale_v_));
    rstream->r(&rotate_, sizeof (rotate_));
    rstream->r(&scroll_u_, sizeof (scroll_u_));
    rstream->r(&scroll_v_, sizeof (scroll_v_));
}
