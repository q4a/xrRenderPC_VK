#include "backend/backend.h"


void
BackEnd::SetShader
        ( ref_shader& shader
        , u32 pass /* = 0 */
        )
{
    SetShader(&*shader, pass);
}


void
BackEnd::SetShader
        ( Shader *shader
        , u32 pass
        )
{
    SetElement(shader->E[0], pass);
}


void
BackEnd::SetElement
        ( ref_selement& element
        , u32 pass /* = 0 */
        )
{
    SetElement(&*element, pass);
}

void
BackEnd::SetElement
        ( ShaderElement *shader
        , u32 pass
        )
{
    // TODO: bind descriptors & PL
}
