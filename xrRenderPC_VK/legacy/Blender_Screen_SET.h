// Blender_Screen_SET.h: interface for the Blender_Screen_SET class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_)
#define AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_
#pragma once

#include "legacy/Blender.h"

class CBlender_Screen_SET : public IBlender
{
    xrP_TOKEN oBlend;
    xrP_Integer oAREF;
    xrP_BOOL oZTest;
    xrP_BOOL oZWrite;
    xrP_BOOL oLighting;
    xrP_BOOL oFog;
    xrP_BOOL oClamp;

public:
    LPCSTR getComment() override { return "basic (simple)"; }
    BOOL canBeLMAPped() override { return FALSE; }
    void Save(IWriter& fs) override;
    void Load(IReader& fs, u16 version) override;
    void Compile(CBlender_Compile& C) override;

    CBlender_Screen_SET();
    virtual ~CBlender_Screen_SET();
};

#endif // !defined(AFX_BLENDER_SCREEN_SET_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_)
