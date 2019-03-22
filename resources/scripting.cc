#include <algorithm>

#include "xrScriptEngine/script_engine.hpp"
#include "luabind/return_reference_to_policy.hpp"

#include "frontend/render.h"
#include "resources/blender_compiler.h"

#include "resources/manager.h"

using namespace luabind;
using namespace luabind::policy;

struct Options
{
    bool MsaaAlphaTestAtoc()
    {
        Msg("! %s: not implemented", __FUNCTION__);
        return false;
    }
};

struct Sampler {};
struct Sampler1 {};

class LuaCompiler
{
    BlenderCompiler &compiler_;
    bool is_first_pass_ = true;

    void TryEndPass()
    {
        if (!is_first_pass_)
        {
            compiler_.PassEnd();
        }
        is_first_pass_ = false;
    }

public:
    LuaCompiler() = delete;
    explicit LuaCompiler(BlenderCompiler &compiler)
        : compiler_(compiler)
    {
    }

    // Bindings
    LuaCompiler &PassBegin( LPCSTR vertex_shader_name
                          , LPCSTR fragment_shader_name
                          )
    {
        // In case if we're starting a second pass
        // need to close the first one manually
        TryEndPass();
        compiler_.PassBegin( vertex_shader_name
                           , "null"
                           , fragment_shader_name
                           );
        return *this;
    }

    LuaCompiler &PassBegin1( LPCSTR vertex_shader_name
                           , LPCSTR geometry_shader_name
                           , LPCSTR fragment_shader_name
                           )
    {
        // In case if we're starting a second pass
        // need to close the first one manually
        TryEndPass();
        compiler_.PassBegin( vertex_shader_name
                           , geometry_shader_name
                           , fragment_shader_name
                           );
        return *this;
    }

    LuaCompiler &SetOptions(int P, bool S)
    {
        Msg("! %s: not implemented", __FUNCTION__); // have no idea yet
        return *this;
    }

    LuaCompiler &SetEmissive(bool value)
    {
        compiler_.emissive = value;
        return *this;
    }

    LuaCompiler &SetDistort(bool value)
    {
        compiler_.distort = value;
        return *this;
    }

    LuaCompiler &SetWallmark(bool value)
    {
        compiler_.wallmark = value;
        return *this;
    }

    LuaCompiler &PassLightFog(bool value)
    {
        compiler_.PassLightFog(false, value);
        return *this;
    }

    LuaCompiler &PassZtest(bool test, bool write)
    {
        compiler_.PassZtest(test, write);
        return *this;
    }

    LuaCompiler &BlendAlphaMode( bool alpha_blend
                               , int source
                               , int destination
                               )
    {
        compiler_.BlendAlphaMode( alpha_blend
                                , vk::BlendFactor(source)
                                , vk::BlendFactor(destination)
        );
        return *this;
    }

    LuaCompiler &BlendAlphaReference(bool alpha_test, int alpha_reference)
    {
        compiler_.BlendAlphaReference(alpha_test, alpha_reference);
        return *this;
    }

    LuaCompiler &BlendColorWriteMask( bool r
                                    , bool g
                                    , bool b
                                    , bool a
                                    )
    {
        compiler_.BlendColorWriteMask(r, g, b, a);
        return *this;
    }

    LuaCompiler &PassTexture(LPCSTR resource_name, LPCSTR texture_name)
    {
        compiler_.PassTexture(resource_name, texture_name);
        return *this;
    }

    LuaCompiler &PassStencil( bool
                            , int
                            , int
                            , int
                            , int
                            , int
                            , int
                            )
    {
        Msg("! %s: not implemented", __FUNCTION__);
        return *this;
    }

    LuaCompiler &StencilReference(int reference)
    {
        Msg("! %s: not implemented", __FUNCTION__);
        return *this;
    }

    LuaCompiler &AlphaToCoverage(bool enable)
    {
        Msg("! %s: not implemented", __FUNCTION__);
        return *this;
    }

    LuaCompiler &Zfunction(int function)
    {
        Msg("! %s: not implemented", __FUNCTION__);
        return *this;
    }

    Sampler Sampler(LPCSTR sampler_name)
    {
        compiler_.PassSampler(sampler_name);
        return {};
    }

    Sampler1 Sampler1(LPCSTR sampler_name)
    {
        Msg("! %s: not implemented", __FUNCTION__);
        return {};
    }

    Options Options()
    {
        return {};
    }
};

struct Blend {};
struct CompareOperation {};
struct StencilOperation {};


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
            class_<Options>("_dx10options")
               .def("dx10_msaa_alphatest_atoc", &Options::MsaaAlphaTestAtoc),

            class_<Sampler>("_dx10sampler"),

            class_<Sampler1>("_sampler"),
                // TODO

            class_<LuaCompiler>("_compiler")
                .def(constructor<const LuaCompiler&>())
                .def("begin",                  &LuaCompiler::PassBegin,           return_reference_to<1>())
                .def("begin",                  &LuaCompiler::PassBegin1,          return_reference_to<1>())
                .def("sorting",                &LuaCompiler::SetOptions,          return_reference_to<1>())
                .def("emissive",               &LuaCompiler::SetEmissive,         return_reference_to<1>())
                .def("distort",                &LuaCompiler::SetDistort,          return_reference_to<1>())
                .def("wmark",                  &LuaCompiler::SetWallmark,         return_reference_to<1>())
                .def("fog",                    &LuaCompiler::PassLightFog,        return_reference_to<1>())
                .def("zb",                     &LuaCompiler::PassZtest,           return_reference_to<1>())
                .def("blend",                  &LuaCompiler::BlendAlphaMode,      return_reference_to<1>())
                .def("aref",                   &LuaCompiler::BlendAlphaReference, return_reference_to<1>())
                .def("dx10color_write_enable", &LuaCompiler::BlendColorWriteMask, return_reference_to<1>())
                .def("color_write_enable",     &LuaCompiler::BlendColorWriteMask, return_reference_to<1>())
                .def("dx10texture",            &LuaCompiler::PassTexture,         return_reference_to<1>())
                .def("dx10stencil",            &LuaCompiler::PassStencil,         return_reference_to<1>())
                .def("dx10stencil_ref",        &LuaCompiler::StencilReference,    return_reference_to<1>())
                .def("dx10atoc",               &LuaCompiler::AlphaToCoverage,     return_reference_to<1>())
                .def("dx10zfunc",              &LuaCompiler::Zfunction,           return_reference_to<1>())

                .def("sampler",                &LuaCompiler::Sampler1)  // returns sampler-object
                .def("dx10sampler",            &LuaCompiler::Sampler)   // returns sampler-object
                .def("dx10Options",            &LuaCompiler::Options),  // returns options-object

            class_<Blend>("blend")
                .enum_("blend")
                [
                    value("zero",         int(vk::BlendFactor::eZero)),
                    value("one",          int(vk::BlendFactor::eOne)),
                    value("srccolor",     int(vk::BlendFactor::eSrcColor)),
                    value("invsrccolor",  int(vk::BlendFactor::eOneMinusSrcColor)),
                    value("srcalpha",     int(vk::BlendFactor::eSrcAlpha)),
                    value("invsrcalpha",  int(vk::BlendFactor::eOneMinusSrcAlpha)),
                    value("destalpha",    int(vk::BlendFactor::eDstAlpha)),
                    value("invdestalpha", int(vk::BlendFactor::eOneMinusDstAlpha)),
                    value("destcolor",    int(vk::BlendFactor::eDstColor)),
                    value("invdestcolor", int(vk::BlendFactor::eOneMinusDstColor)),
                    value("srcalphasat",  int(vk::BlendFactor::eSrcAlphaSaturate))
                ],
           class_<CompareOperation>("cmp_func")
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

            class_<StencilOperation>("stencil_op")
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
    auto folder_list = FS.file_list_open( "$game_shaders$"
                                        , frontend.getShaderPath()
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
 * \brief   Replaces backslahes by undescores in shader name
 */
static std::string
UndecorateShaderName(const std::string &shader_name)
{
    std::string name_undecorated;
    name_undecorated.resize(shader_name.size());

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

    return name_undecorated;
}


/*!
 * \brief   Checks if given shader registered in the script engine
 */
bool
ResourceManager::ScriptingHasShader
        ( const std::string &shader_name
        )
{
    const auto &name_undecorated = UndecorateShaderName(shader_name);

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
    const auto &name_undecorated = UndecorateShaderName(shader_name);

    auto shader_resource = std::make_shared<Shader>();
    shader_resource->set_name(shader_name.c_str());
    shader_resource->dwFlags |= xr_resource_flagged::RF_REGISTERED;

    // Initialize compiler
    BlenderCompiler compiler;
    ParseList(textures, compiler.textures);
    RemoveTexturesExtension(compiler.textures);

    // Compile element (LOD0 - HQ)
    if (script_engine_.object( name_undecorated.c_str()
                             , "normal_hq"
                             , LUA_TFUNCTION
                             )
       )
    {
        compiler.current_element = ShaderElementType::NormalHq;
        compiler.detail =
            texture_description_.GetDetailTexture( compiler.textures[0]
                                                 , compiler.detail_texture
                                                 , compiler.detail_scaler
            );

        shader_resource->elements[0] =
            compiler.CompileLua( name_undecorated
                               , compiler.detail ? "normal_hq" : "normal"
            );
    }
    else if (script_engine_.object( name_undecorated.c_str()
                                  , "normal"
                                  , LUA_TFUNCTION
                                  )
            )
    {
        compiler.current_element = ShaderElementType::NormalHq;
        compiler.detail =
            texture_description_.GetDetailTexture( compiler.textures[0]
                                                 , compiler.detail_texture
                                                 , compiler.detail_scaler
            );

        shader_resource->elements[0] =
            compiler.CompileLua(name_undecorated, "normal");
    }

    // Compile element (LOD1 - Normal)
    if (script_engine_.object( name_undecorated.c_str()
                             , "normal"
                             , LUA_TFUNCTION
                             )
       )
    {
        compiler.current_element = ShaderElementType::NormalLq;
        compiler.detail =
            texture_description_.GetDetailTexture( compiler.textures[0]
                                                 , compiler.detail_texture
                                                 , compiler.detail_scaler
            );
        shader_resource->elements[1] =
            compiler.CompileLua(name_undecorated, "normal");
    }

    // Compile element (LOD2)
    if (script_engine_.object( name_undecorated.c_str()
                             , "l_point"
                             , LUA_TFUNCTION
                             )
       )
    {
        compiler.current_element = ShaderElementType::LightPoint;
        compiler.detail = false;
        shader_resource->elements[2] =
            compiler.CompileLua(name_undecorated, "l_point");
    }

    // Compile element (LOD3)
    if (script_engine_.object( name_undecorated.c_str()
                             , "l_spot"
                             , LUA_TFUNCTION
                             )
       )
    {
        compiler.current_element = ShaderElementType::LightSpot;
        compiler.detail = false;
        shader_resource->elements[3] =
            compiler.CompileLua(name_undecorated, "l_spot");
    }

    // Compile element (LOD4)
    if (script_engine_.object( name_undecorated.c_str()
                             , "l_special"
                             , LUA_TFUNCTION
                             )
       )
    {
        compiler.current_element = ShaderElementType::LightSpecial;
        compiler.detail = false;
        shader_resource->elements[4] =
            compiler.CompileLua(name_undecorated, "l_special");
    }

    // Check if we already have this shader created
    const auto predicate = [&](const auto &shader) -> bool
    {
        return shader_resource->IsEqual(*shader);
    };
    const auto &iterator =
        std::find_if( shaders_.cbegin()
                    , shaders_.cend()
                    , predicate
        );

    if (iterator != shaders_.cend())
    {
        return *iterator;
    }

    shaders_.push_back(shader_resource);
    return shader_resource;
}


/*!
 * \brief   Deinitializes scripting engine
 */
void
ResourceManager::ScriptingUnload()
{
    script_engine_.unload();
}


//-----------------------------------------------------------------------------
std::shared_ptr<ShaderElement>
BlenderCompiler::CompileLua
        ( const std::string &name_space
        , const std::string &shader_name
        )
{
    shader_element = std::make_shared<ShaderElement>();

    const object shader =
        resources.script_engine_.name_space(name_space.c_str());
    const functor<void> lua_element = (object)shader[shader_name.c_str()];
    LuaCompiler lua_compiler{ *this };
 
    LPCSTR texture0 = textures.size()       ? textures[0].c_str()    : "null";
    LPCSTR texture1 = textures.size() > 1   ? textures[1].c_str()    : "null";
    LPCSTR detail   = detail_texture.size() ? detail_texture.c_str() : "null";

    lua_element( lua_compiler
               , texture0
               , texture1
               , detail
    );

    // Close last pass
    PassEnd();

    return resources.CreateShaderElement(*shader_element);
}
