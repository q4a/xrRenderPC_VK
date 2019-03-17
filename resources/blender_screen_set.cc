#include "Layers/xrRender/blenders/Blender_CLSID.h"

#include "resources/blender_screen_set.h"

using namespace blenders;

enum BlendMode
{
    SET,
    BLEND,
    ADD,
    MULTIPLY,
    MULTIPLY_2X,
    ALPHA_ADD,
    MULTIPLY_2X_BD, // TODO: all names aren't clear yet
    SET_2R,
    BLEND_2R,
    BLEND_4R
};


enum BlendCount
{
    VER_2   =  7,
    VER_4   =  9,
    VER_5   = 10
};


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

    blend_.Count      = VER_4;
    blend_.IDselected = SET;

    aref_.value =  32;
    aref_.min   =   0;
    aref_.max   = 255;

    ztest_.value  = false;
    zwrite_.value = false;

    lighting_.value = false;
    fog_.value      = false;

    clamp_.value = true;
}


/**
 *
 */
void
ScreenSet::Load
        ( IReader &rstream
        , u16      version
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

    blend_.Count = VER_5;
}


/**
 *
 */
void
ScreenSet::Save
        ( IWriter &wstream
        )
{
    Blender::Save(wstream);

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
    case MULTIPLY_2X_BD:
        compiler.PassBegin("stub_notransform_t", "stub_default_ma");
        break;
    case SET_2R:
        // no break
    case BLEND_2R:
        compiler.PassBegin("stub_notransform_t_m2", "stub_default");
        break;
    case BLEND_4R:
        compiler.PassBegin("stub_notransform_t_m4", "stub_default");
        break;
    default:
        compiler.PassBegin("stub_notransform_t", "stub_default");
        break;
    };

    compiler.PassTexture("s_base", compiler.textures[0]);
    compiler.PassSampler("smp_base");
    if (clamp_.value)
    {
        compiler.SamplerAddressing( "smp_base"
                                  , vk::SamplerAddressMode::eClampToEdge
        );
    }

    compiler.PassZtest(ztest_.value, zwrite_.value);

    switch (blend_.IDselected)
    {
    case SET:
        compiler.PassBlend( false
                          , vk::BlendFactor::eOne
                          , vk::BlendFactor::eZero
                          , false
                          , 0
        );
        break;
    case BLEND:
        compiler.PassBlend( true
                          , vk::BlendFactor::eSrcAlpha
                          , vk::BlendFactor::eOneMinusSrcAlpha
                          , true
                          , aref_.value
        );
        break;
    case ADD:
        compiler.PassBlend( true
                          , vk::BlendFactor::eOne
                          , vk::BlendFactor::eOne
                          , false
                          , aref_.value
        );
        break;
    case MULTIPLY:
        compiler.PassBlend( true
                          , vk::BlendFactor::eDstColor
                          , vk::BlendFactor::eZero
                          , false
                          , aref_.value
        );
        break;
    case MULTIPLY_2X:
        compiler.PassBlend( true
                          , vk::BlendFactor::eDstColor
                          , vk::BlendFactor::eSrcColor
                          , false
                          , aref_.value
        );
        break;
    case ALPHA_ADD:
        compiler.PassBlend( true
                          , vk::BlendFactor::eSrcAlpha
                          , vk::BlendFactor::eOne
                          , true
                          , aref_.value
        );
        break;
    case MULTIPLY_2X_BD:
        compiler.PassBlend( true
                          , vk::BlendFactor::eDstColor
                          , vk::BlendFactor::eSrcColor
                          , false
                          , aref_.value
        );
        break;
    case SET_2R:
        compiler.PassBlend( true
                          , vk::BlendFactor::eOne
                          , vk::BlendFactor::eOne
                          , true
                          , 0
        );
        break;
    case BLEND_2R:
        // no break
    case BLEND_4R:
        compiler.PassBlend( true
                          , vk::BlendFactor::eSrcAlpha
                          , vk::BlendFactor::eOneMinusSrcAlpha
                          , true
                          , aref_.value
        );
        break;
    default:
        R_ASSERT(false);
        break;
    }

    compiler.PassLightingFog(lighting_.value, fog_.value);
    compiler.PassEnd();
}
