#include "factory/console/render.h"


/**
 *
 */
void
fConsoleRender::Copy
        ( IConsoleRender& obj
        )
{
    *this = *((fConsoleRender *)&obj);
}


/**
 *
 */
void
fConsoleRender::OnRender
        ( bool inGame
        )
{
    // TBI
}
