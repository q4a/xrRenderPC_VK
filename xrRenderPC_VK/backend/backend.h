#ifndef BACKEND_BACKEND_H_
#define BACKEND_BACKEND_H_

#include "common.h"
#include "legacy/Shader.h"

class BackEnd
{
public:
    BackEnd() = default;
    ~BackEnd() = default;

    void SetElement(ShaderElement *element, u32 pass = 0);
    void SetElement(ref_selement& element, u32 pass = 0);

    void SetShader(Shader *shader, u32 pass = 0);
    void SetShader(ref_shader& shader, u32 pass = 0);
};

extern BackEnd backend;

#endif // BACKEND_BACKEND_H_
