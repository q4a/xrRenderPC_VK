#include "frontend/render.h"
#include "resources/blender_screen_set.h"

#include "resources/blender_compiler.h"

void
BlenderCompiler::Compile
        ( Element &element
        )
{
    // TBI
    blender->Compile(*this);
}

void
BlenderCompiler::PassBegin
        ( const std::string &vertex_shader
        , const std::string &fragment_shader
        )
{
    // Create shaders
    vs = frontend.resources_->CreateVertexShader(vertex_shader);
    fs = frontend.resources_->CreateFragmentShader(fragment_shader);

    // Merge constant tables
    // TODO
}


void
BlenderCompiler::PassTexture
        ( const std::string &name
        , const std::string &texture
        )
{

}


void
BlenderCompiler::PassEnd()
{

}
