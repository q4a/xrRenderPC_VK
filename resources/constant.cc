#include "resources/constant.h"

/**
 *
 */
void
Constant::Load
        ( IReader *rstream
        )
{
    rstream->r(&r_, sizeof (r_));
    rstream->r(&g_, sizeof (g_));
    rstream->r(&b_, sizeof (b_));
    rstream->r(&a_, sizeof (a_));
}
