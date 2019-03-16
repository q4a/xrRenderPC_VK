#include "common.h"
#include "frontend/render.h"
#include "legacy/Blender.h"
#include "legacy/Blender_CLSID.h"
#include "legacy/Blender_Screen_SET.h"

IBlender* FrontEnd::blender_create(CLASS_ID cls)
{
    switch (cls)
    {
    case B_DEFAULT: return 0;//new CBlender_deffer_flat();
    case B_DEFAULT_AREF: return 0;// new CBlender_deffer_aref(true);
    case B_VERT: return 0;// new CBlender_deffer_flat();
    case B_VERT_AREF: return 0;// new CBlender_deffer_aref(false);
    case B_SCREEN_SET: return new CBlender_Screen_SET();
    case B_SCREEN_GRAY: return 0;
    case B_EDITOR_WIRE: return 0;// new CBlender_Editor_Wire();
    case B_EDITOR_SEL: return 0; // new CBlender_Editor_Selection();
    case B_LIGHT: return 0;
    case B_LmBmmD: return 0; // new CBlender_BmmD();
    case B_LaEmB: return 0;
    case B_LmEbB: return 0; // new CBlender_LmEbB();
    case B_B: return 0;
    case B_BmmD: return 0;// new CBlender_BmmD();
    case B_SHADOW_TEX: return 0;
    case B_SHADOW_WORLD: return 0;
    case B_BLUR: return 0;
    case B_MODEL: return 0; // new CBlender_deffer_model();
    case B_MODEL_EbB: return 0;// new CBlender_Model_EbB();
    case B_DETAIL: return 0;// new CBlender_Detail_Still();
    case B_TREE: return 0;// new CBlender_Tree();
    case B_PARTICLE: return 0;// new CBlender_Particle();
    }
    return 0;
}

void FrontEnd::blender_destroy(IBlender*& B) { xr_delete(B); }