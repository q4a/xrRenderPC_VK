#include "Layers/xrRender/blenders/Blender_CLSID.h"

#include "resources/blender_screen_set.h"

using namespace blenders;


/**
 *
 */
LPCSTR
ScreenSet::getName()
{
    return Blender::getName();
}


/**
 *
 */
LPCSTR
ScreenSet::getComment()
{
    return "basic (simple)";
}


/**
 *
 */
ScreenSet::ScreenSet()
{
    description.class_id = B_SCREEN_SET;
    description.version  = 4;
    // TBI
}


/**
 *
 */
void
ScreenSet::Load
        ( IReader &rstream
        , u16 version
        )
{
    Blender::Load(rstream, version);

    switch (version)
    {
    case 2:
        xrPREAD_PROP(rstream, xrPID_TOKEN,      blend_    );
        xrPREAD_PROP(rstream, xrPID_INTEGER,    aref_     );
        xrPREAD_PROP(rstream, xrPID_BOOL,       ztest_    );
        xrPREAD_PROP(rstream, xrPID_BOOL,       zwrite_   );
        xrPREAD_PROP(rstream, xrPID_BOOL,       lighting_ );
        xrPREAD_PROP(rstream, xrPID_BOOL,       fog_      );
        break;

    case 3:
        xrPREAD_PROP(rstream, xrPID_TOKEN,      blend_    );
        xrPREAD_PROP(rstream, xrPID_BOOL,       clamp_    );
        xrPREAD_PROP(rstream, xrPID_INTEGER,    aref_     );
        xrPREAD_PROP(rstream, xrPID_BOOL,       ztest_    );
        xrPREAD_PROP(rstream, xrPID_BOOL,       zwrite_   );
        xrPREAD_PROP(rstream, xrPID_BOOL,       lighting_ );
        xrPREAD_PROP(rstream, xrPID_BOOL,       fog_      );
        break;

    default:
        xrPREAD_PROP(rstream, xrPID_TOKEN,      blend_    );
        xrPREAD_PROP(rstream, xrPID_BOOL,       clamp_    );
        xrPREAD_PROP(rstream, xrPID_INTEGER,    aref_     );
        xrPREAD_PROP(rstream, xrPID_BOOL,       ztest_    );
        xrPREAD_PROP(rstream, xrPID_BOOL,       zwrite_   );
        xrPREAD_PROP(rstream, xrPID_BOOL,       lighting_ );
        xrPREAD_PROP(rstream, xrPID_BOOL,       fog_      );
        break;
    }

    blend_.Count = 10; // ver5
}


/**
 *
 */
void
ScreenSet::Save
        ( IWriter &wstream
        )
{
    // TBI
}
