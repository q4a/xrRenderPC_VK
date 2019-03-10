
#include "backend/backend.h"
#include "console.h"
#include "debug/render.h"
#include "factory/factory.h"
#include "frontend/render.h"
#include "resources/manager.h"
#include "ui/render.h"


/*!
 * Statically created objects
 */
BackEnd         backend;
FrontEnd        frontend;
ResourceManager resources;
RenderFactory   factory;
DebugRender     debug;


/*
 * TODO:
 * Current implementation uses a static library provided by LunarG SDK.
 * It's a good idea to replace it by ptr calls using 3rd party loaders.
 *
 * E.g. https://github.com/zeux/volk
 */
#pragma comment(lib, "vulkan-1")

extern "C"
{
XR_EXPORT void SetupEnv()
{
    GEnv.Render         = &frontend;
    GEnv.RenderFactory  = &factory;
    GEnv.DU             = nullptr;
    GEnv.UIRender       = &ui;
#ifdef DEBUG
    GEnv.DRender        = &debug;
#endif
    InitConsole();
}

XR_EXPORT pcstr GetModeName()
{
    return "renderer_vk";
}

XR_EXPORT bool CheckRendererSupport()
{
    const auto apiVersion = vk::enumerateInstanceVersion();
    Msg( "* Vulkan Instance ver. %d.%d.%d"
       , VK_VERSION_MAJOR(apiVersion)
       , VK_VERSION_MINOR(apiVersion)
       , VK_VERSION_PATCH(apiVersion)
    );
    return (apiVersion >= VK_API_VERSION_1_1);
}
}
