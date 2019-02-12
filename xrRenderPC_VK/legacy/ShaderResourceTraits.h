#pragma once

#include "frontend/render.h"


template <typename T>
struct ShaderTypeTraits;

template <>
struct ShaderTypeTraits<SVS>
{
    using MapType = ResourceManager::map_VS;

    static inline const char* GetShaderExt() { return ".vs"; }
    static inline const char* GetCompilationTarget()
    {
        return "vs_2_0";
    }

    static void GetCompilationTarget(const char*& target, const char*& entry, const char* data)
    {
        target = "vs_2_0";

        if (strstr(data, "main_vs_1_1"))
        {
            target = "vs_1_1";
            entry = "main_vs_1_1";
        }

        if (strstr(data, "main_vs_2_0"))
        {
            target = "vs_2_0";
            entry = "main_vs_2_0";
        }
    }

#if 0
#ifdef USE_OGL
    using HWShaderType = GLuint;
#else
    using HWShaderType = ID3DVertexShader * ;
#endif

    static inline HRESULT CreateHWShader(DWORD const* buffer, size_t size, HWShaderType& sh)
    {
        HRESULT res = 0;

#ifdef USE_OGL
        sh = glCreateShader(GL_VERTEX_SHADER);
#elif defined(USE_DX11)
        res = HW.pDevice->CreateVertexShader(buffer, size, 0, &sh);
#elif defined(USE_DX10)
        res = HW.pDevice->CreateVertexShader(buffer, size, &sh);
#else
        res = HW.pDevice->CreateVertexShader(buffer, &sh);
#endif

        return res;
    }

    static inline u32 GetShaderDest() { return RC_dest_vertex; }
#endif
};

template <>
struct ShaderTypeTraits<SPS>
{
    using MapType = ResourceManager::map_PS;

    static inline const char* GetShaderExt() { return ".ps"; }
    static inline const char* GetCompilationTarget()
    {
        return "ps_2_0";
    }

    static void GetCompilationTarget(const char*& target, const char*& entry, const char* data)
    {
        if (strstr(data, "main_ps_1_1"))
        {
            target = "ps_1_1";
            entry = "main_ps_1_1";
        }
        if (strstr(data, "main_ps_1_2"))
        {
            target = "ps_1_2";
            entry = "main_ps_1_2";
        }
        if (strstr(data, "main_ps_1_3"))
        {
            target = "ps_1_3";
            entry = "main_ps_1_3";
        }
        if (strstr(data, "main_ps_1_4"))
        {
            target = "ps_1_4";
            entry = "main_ps_1_4";
        }
        if (strstr(data, "main_ps_2_0"))
        {
            target = "ps_2_0";
            entry = "main_ps_2_0";
        }
    }

#if 0
#ifdef USE_OGL
    using HWShaderType = GLuint;
#else
    using HWShaderType = ID3DPixelShader * ;
#endif

    static inline HRESULT CreateHWShader(DWORD const* buffer, size_t size, HWShaderType& sh)
    {
        HRESULT res = 0;

#ifdef USE_OGL
        sh = glCreateShader(GL_FRAGMENT_SHADER);
#elif defined(USE_DX11)
        res = HW.pDevice->CreatePixelShader(buffer, size, 0, &sh);
#elif defined(USE_DX10)
        res = HW.pDevice->CreatePixelShader(buffer, size, &sh);
#else
        res = HW.pDevice->CreatePixelShader(buffer, &sh);
#endif

        return res;
    }

    static inline u32 GetShaderDest() { return RC_dest_pixel; }
#endif
};



template <>
inline ResourceManager::map_PS& ResourceManager::GetShaderMap()
{
    return m_ps;
}

template <>
inline ResourceManager::map_VS& ResourceManager::GetShaderMap()
{
    return m_vs;
}

#if 0
template <typename T>
bool CResourceManager::DestroyShader(const T* sh)
{
    if (0 == (sh->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return false;

   typename ShaderTypeTraits<T>::MapType& sh_map = GetShaderMap<typename ShaderTypeTraits<T>::MapType>();

    LPSTR N = LPSTR(*sh->cName);
    auto iterator = sh_map.find(N);

    if (iterator != sh_map.end())
    {
        sh_map.erase(iterator);
        return true;
    }

    Msg("! ERROR: Failed to find compiled shader '%s'", sh->cName.c_str());
    return false;
}

#endif
