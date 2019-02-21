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


/**
 *
 */
void
ScreenSet::Compile
        ( BlenderCompiler &compiler
        )
{
    Blender::Compile(compiler);

    switch (blend_.IDselected)
    {
    case 1:
        compiler.PassBegin("stub_notransform_t", "stub_default");
        break;
    default:
        break;
    };

    compiler.PassTexture("s_base", compiler.textures[0]);
    compiler.PassSampler("smp_base");
    compiler.PassZtest(ztest_.value, zwrite_.value);

    switch (blend_.IDselected)
    {
    case 1: // BLEND
        compiler.PassBlend( true
                          , 0/*D3DBLEND_SRCALPHA*/
                          , 1/*D3DBLEND_INVSRCALPHA*/
                          , true
                          , aref_.value
        );
        break;
    default:
        break;
    }

    compiler.PassLightingFog(lighting_.value, fog_.value);
    compiler.PassEnd();
}
