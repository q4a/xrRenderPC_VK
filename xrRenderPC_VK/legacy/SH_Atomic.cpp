#include "stdafx.h"
#pragma hdrstop

#include "legacy/SH_Atomic.h"
#include "frontend/render.h"

///////////////////////////////////////////////////////////////////////
// SPS
SPS::~SPS()
{
#if 0
#ifdef USE_OGL
    CHK_GL(glDeleteProgram(sh));
#else
    _RELEASE(sh);
#endif
    
    RImplementation.Resources->_DeletePS(this);
#endif
}

#ifndef USE_DX9
///////////////////////////////////////////////////////////////////////
// SGS
SGS::~SGS()
{
#if 0
#ifdef USE_OGL
    CHK_GL(glDeleteProgram(sh));
#else
    _RELEASE(sh);
#endif

    RImplementation.Resources->_DeleteGS(this);
#endif
}

#if defined(USE_DX11)
SHS::~SHS()
{
#ifdef USE_OGL
    CHK_GL(glDeleteProgram(sh));
#else
    _RELEASE(sh);
#endif

    RImplementation.Resources->_DeleteHS(this);
}

SDS::~SDS()
{
#ifdef USE_OGL
    CHK_GL(glDeleteProgram(sh));
#else
    _RELEASE(sh);
#endif

    RImplementation.Resources->_DeleteDS(this);
}

SCS::~SCS()
{
#ifdef USE_OGL
    CHK_GL(glDeleteProgram(sh));
#else
    _RELEASE(sh);
#endif

    RImplementation.Resources->_DeleteCS(this);
}
#endif
#endif // USE_DX10

#if defined(USE_DX10) || defined(USE_DX11)
///////////////////////////////////////////////////////////////////////
//	SInputSignature
SInputSignature::SInputSignature(ID3DBlob* pBlob)
{
    VERIFY(pBlob);
    signature = pBlob;
    signature->AddRef();
};

SInputSignature::~SInputSignature()
{
    _RELEASE(signature);
    RImplementation.Resources->_DeleteInputSignature(this);
}
#endif	//	USE_DX10

///////////////////////////////////////////////////////////////////////
//	SState
SState::~SState()
{
#if 0
#ifdef USE_OGL
    state.Release();
#else // USE_OGL
    _RELEASE(state);
#endif // USE_OGL
    RImplementation.Resources->_DeleteState(this);
#endif
}

///////////////////////////////////////////////////////////////////////
//	SDeclaration
SDeclaration::~SDeclaration()
{
#if 0
    RImplementation.Resources->_DeleteDecl(this);
    //	Release vertex layout
#ifdef USE_OGL
    glDeleteBuffers(1, &dcl);
#elif defined(USE_DX10) || defined(USE_DX11)
    xr_map<ID3DBlob*, ID3DInputLayout*>::iterator iLayout;
    iLayout = vs_to_layout.begin();
    for (; iLayout != vs_to_layout.end(); ++iLayout)
    {
        //	Release vertex layout
        _RELEASE(iLayout->second);
    }
#else // USE_DX10
    _RELEASE(dcl);
#endif // USE_DX10
#endif
}
