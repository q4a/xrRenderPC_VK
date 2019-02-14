#include "resources/blender.h"


/**
 *
 */
BlenderDescription &
Blender::GetDescription()
{
    return description;
}


/**
 *
 */
LPCSTR
Blender::getName()
{
    return description.name;
}


/**
 *
 */
void
Blender::Load
        ( IReader &rstream
        , u16 version
        )
{
    rstream.r(&description, sizeof (description));

    xrPREAD_MARKER(rstream);
    xrPREAD_PROP  (rstream, xrPID_INTEGER,   priority       );
    xrPREAD_PROP  (rstream, xrPID_BOOL,      strict_sorting );
    xrPREAD_MARKER(rstream);
    xrPREAD_PROP  (rstream, xrPID_TEXTURE,   name           );
    xrPREAD_PROP  (rstream, xrPID_MATRIX,    xform          );
}


/**
 *
 */
void
Blender::Save
        ( IWriter &wstream
        )
{
    // TBI
}
