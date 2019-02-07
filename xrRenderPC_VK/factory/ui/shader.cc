#include "factory/ui/shader.h"


/**
 *
 */
void
fUIShader::Copy
        ( IUIShader& obj
        )
{
    *this = *((fUIShader *)&obj);
}


/**
 *
 */
void
fUIShader::create
        ( LPCSTR shaderName
        , LPCSTR textureList
        )
{
    // TBI
}

/**
 *
 */
bool
fUIShader::inited()
{
    // TBI
    return false;
}


/**
 *
 */
void
fUIShader::destroy()
{
    // TBI
}
