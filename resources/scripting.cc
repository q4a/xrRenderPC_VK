#include <algorithm>

#include "xrScriptEngine/script_engine.hpp"

#include "frontend/render.h"

#include "resources/manager.h"

using namespace luabind;

struct cmp_func {};
struct stencil_op {};


/*!
 * \brief   Initializes scripting system and sets up bindings
 */
void
ResourceManager::ScriptingLoad()
{
    auto exporterFunc = [](lua_State *luaState)
    {
        module(luaState)
        [
           class_<cmp_func>("cmp_func")
                .enum_("cmp_func")
                [
                    value("never",        int(vk::CompareOp::eNever)),
                    value("less",         int(vk::CompareOp::eLess)),
                    value("equal",        int(vk::CompareOp::eEqual)),
                    value("lessequal",    int(vk::CompareOp::eLessOrEqual)),
                    value("greater",      int(vk::CompareOp::eGreater)),
                    value("notequal",     int(vk::CompareOp::eNotEqual)),
                    value("greaterequal", int(vk::CompareOp::eGreaterOrEqual)),
                    value("always",       int(vk::CompareOp::eAlways))
                ],

            class_<stencil_op>("stencil_op")
                .enum_("stencil_op")
                [
                    value("keep",    int(vk::StencilOp::eKeep)),
                    value("zero",    int(vk::StencilOp::eZero)),
                    value("replace", int(vk::StencilOp::eReplace)),
                    value("incrsat", int(vk::StencilOp::eIncrementAndClamp)),
                    value("decrsat", int(vk::StencilOp::eDecrementAndClamp)),
                    value("invert",  int(vk::StencilOp::eInvert)),
                    value("incr",    int(vk::StencilOp::eIncrementAndWrap)),
                    value("decr",    int(vk::StencilOp::eDecrementAndWrap))
                ]
        ];
    };

    script_engine_.init(exporterFunc, false);

    // Load shaders names
    const auto shaderPath = frontend.getShaderPath();
    auto folder_list = FS.file_list_open( "$game_shaders$"
                                        , shaderPath
                                        , FS_ListFiles | FS_RootOnly
    );

    // Create shaders namespaces
    for (const auto &file_name : *folder_list)
    {
        std::string name_space{ file_name };
        std::string extension{ strext(file_name) };
        if (   extension.empty()
            || xr_strcmp(extension.c_str(), ".s")
           )
        {
            continue;
        }

        const auto &iterator = std::find_end( name_space.cbegin()
                                            , name_space.cend()
                                            , extension.cbegin()
                                            , extension.cend()
        );

        name_space.resize(std::distance(name_space.cbegin(), iterator));
        if (name_space.empty())
        {
            name_space = script_engine_.GlobalNamespace;
        }

        string_path full_path;
        strconcat( sizeof(full_path)
                 , full_path
                 , frontend.getShaderPath()
                 , file_name
        );
        FS.update_path(full_path, "$game_shaders$", full_path);
        script_engine_.load_file_into_namespace(full_path, name_space.c_str());
    }
    FS.file_list_close(folder_list);
}


/*!
 * \brief   Checks if given shader registered in the script engine
 */
bool
ResourceManager::ScriptingHasShader
        ( const std::string &shader_name
        )
{
    std::string name_undecorated = shader_name;

    auto undecorate = [](const char c) -> char
    {
        if (c == '\\')
            return '_';
        return c;
    };

    std::transform( shader_name.cbegin()
                  , shader_name.cend()
                  , name_undecorated.begin()
                  , undecorate
    );

    const bool has_normal =
        script_engine_.object( name_undecorated.c_str()
                             , "normal"
                             , LUA_TFUNCTION
        );

    const bool has_special =
        script_engine_.object( name_undecorated.c_str()
                             , "l_special"
                             , LUA_TFUNCTION
        );


    return (has_normal || has_special);
}


/*!
 * \brief   Creates shader object from LUA script
 */
std::shared_ptr<Shader>
ResourceManager::CreateShaderLua
        ( const std::string &shader_name
        , const std::string &textures
        )
{
    // TBI
    return {};
}


/*!
 * \brief   Deinitializes scripting engine
 */
void
ResourceManager::ScriptingUnload()
{
    script_engine_.unload();
}
