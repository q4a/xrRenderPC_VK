#include "resources/manager.h"

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
        ( LPCSTR shader_name
        , LPCSTR texture_list
        )
{
    shader_ = resources.CreateShader(shader_name, texture_list);
}

/**
 *
 */
bool
fUIShader::inited()
{
    return (shader_ != nullptr);
}


/**
 *
 */
void
fUIShader::destroy()
{
    resources.DestroyShader(shader_);
}
