#ifndef FRONTEND_RENDER_H_
#define FRONTEND_RENDER_H_

#include <memory>
#include <vector>

#include "xrEngine/pure.h"
#include "xrEngine/Render.h"

#include "resources/manager.h"

class FrontEnd
    : public IRender
    , public pureFrame
{
public:
    FrontEnd() = default;
    ~FrontEnd() = default;

    void OnFrame() override {} // TBI

    // feature level
    GenerationLevel get_generation() override { return GENERATION_R1; } // TBI

    bool is_sun_static() override  { return true; } // TBI
    DWORD get_dx_level() override { return 0; } // TBI

    // Loading / Unloading
    void create() override {} // TBI
    void destroy() override {} // TBI
    void reset_begin() override {} // TBI
    void reset_end() override {} // TBI
    void level_Load(IReader* fs) override {} // TBI
    void level_Unload() override {} // TBI

    HRESULT shader_compile(LPCSTR name, IReader* fs, LPCSTR function_name,
        LPCSTR target, DWORD flags, void *&result) override { return E_FAIL; } // TBI

    // Information
    void DumpStatistics(IGameFont& font, IPerformanceAlert* alert) override {} // TBI

    LPCSTR getShaderPath() override;
    
    IRender_Sector* getSector(int id) override { return nullptr; } // TBI
    IRenderVisual* getVisual(int id) override { return nullptr; } // TBI
    IRender_Sector* detectSector(const Fvector& P) override { return nullptr; } // TBI
    IRender_Target* getTarget() override { return nullptr; } // TBI

    // Main
    void set_Transform(Fmatrix* M) override {} // TBI
    void set_HUD(BOOL V) override {} // TBI
    BOOL get_HUD() override { return false; } // TBI
    void set_Invisible(BOOL V) override {} // TBI
    void flush() override {} // TBI
    void set_Object(IRenderable* O) override {} // TBI
    void add_Occluder(Fbox2& bb_screenspace) override {} // TBI
    void add_Visual(IRenderVisual* V) override {} // TBI
    void add_Geometry(IRenderVisual* V) override {} // TBI
    void add_StaticWallmark(const wm_shader& S, const Fvector& P, float s,
        CDB::TRI* T, Fvector* V) override {} // TBI
    void add_StaticWallmark(IWallMarkArray* pArray, const Fvector& P, float s,
        CDB::TRI* T, Fvector* V) override {} // TBI
    void clear_static_wallmarks() override {} // TBI
    void add_SkeletonWallmark(const Fmatrix* xf, IKinematics* obj,
        IWallMarkArray* array, const Fvector& start, const Fvector& dir,
        float size) override {} // TBI

    Blender* blender_create(CLASS_ID cls);
    void blender_destroy(Blender* &);

    IRender_ObjectSpecific* ros_create(IRenderable* parent) override { return nullptr; } // TBI
    void ros_destroy(IRender_ObjectSpecific*&) override {} // TBI

    // Lighting/glowing
    IRender_Light* light_create() override { return nullptr; } // TBI
    void light_destroy(IRender_Light* p_) override {} // TBI
    IRender_Glow* glow_create() override { return nullptr; } // TBI
    void glow_destroy(IRender_Glow* p_) override {} // TBI

    // Models
    IRenderVisual* model_CreateParticles(LPCSTR name) override { return nullptr; } // TBI
    IRenderVisual* model_Create(LPCSTR name, IReader* data = 0) override { return nullptr; } // TBI
    IRenderVisual* model_CreateChild(LPCSTR name, IReader* data) override { return nullptr; } // TBI
    IRenderVisual* model_Duplicate(IRenderVisual* V) override { return nullptr; } // TBI
    void model_Delete(IRenderVisual*& visual, BOOL discard = FALSE) override {} // TBI
    void model_Logging(BOOL enable) override {} // TBI
    void models_Prefetch() override {} // TBI
    void models_Clear(BOOL complete) override {} // TBI

    // Occlusion culling
    BOOL occ_visible(vis_data& V) override { return false; } // TBI
    BOOL occ_visible(Fbox& B) override { return false; } // TBI
    BOOL occ_visible(sPoly& P) override { return false; } // TBI

    // Main
    void Calculate() override {} // TBI
    void Render() override {} // TBI

    void BeforeWorldRender() override {} // TBI
    void AfterWorldRender() override {} // TBI

    void Screenshot(ScreenshotMode mode = SM_NORMAL, LPCSTR name = 0) override {} // TBI
    void Screenshot(ScreenshotMode mode, CMemoryWriter& memory_writer) override {} // TBI
    void ScreenshotAsyncBegin() override {} // TBI
    void ScreenshotAsyncEnd(CMemoryWriter& memory_writer) override {} // TBI

    // Render mode
    void rmNear() override {} // TBI
    void rmFar() override {} // TBI
    void rmNormal() override {} // TBI
    u32 active_phase() override { return 0; } // TBI

    void Copy(IRender& obj) override {} // TBI

    // Gamma correction functions
    void setGamma(float gamma) override {} // TBI
    void setBrightness(float gamma) override {} // TBI
    void setContrast(float gamma) override {} // TBI
    void updateGamma() override {} // TBI

    // Destroy
    void OnDeviceDestroy(bool keep_textures) override;
    void ValidateHW() override {}
    void DestroyHW() override;
    void Reset(SDL_Window* hwnd, u32& width, u32& height, float& width_2, float& height_2) override {} // TBI

    // Init
    void SetupStates() override {} // TBI
    void OnDeviceCreate(LPCSTR shader) override;
    void Create(SDL_Window* hwnd, u32& width, u32& height, float& width_2,
        float& height_2) override;
    void SetupGPU(bool force_sw, bool force_nonpure, bool force_ref) override;
    
    // Overdraw
    void overdrawBegin() override {} // TBI
    void overdrawEnd() override {} // TBI

    // Resources control
    void DeferredLoad(bool E) override {} // TBI
    void ResourcesDeferredUpload() override {} // TBI
    void ResourcesGetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps) override {} // TBI
    void ResourcesDestroyNecessaryTextures() override {} // TBI
    void ResourcesStoreNecessaryTextures() override {} // TBI
    void ResourcesDumpMemoryUsage() override {} // TBI

    // HWSupport
    bool HWSupportsShaderYUV2RGB() override { return false; } // TBI

    // Device state
    DeviceState GetDeviceState() override;
    bool GetForceGPU_REF() override;
    u32 GetCacheStatPolys() override { return 0; } // TBI
    void Begin() override;
    void Clear() override {} // TBI
    void End() override;
    void ClearTarget() override {} // TBI
    void SetCacheXform(Fmatrix& view, Fmatrix& projection) override {} // TBI
    void OnAssetsChanged() override {} // TBI

protected:
    void ScreenshotImpl(ScreenshotMode mode, LPCSTR name, CMemoryWriter* memory_writer) override {} // TBI

private:
    std::uint32_t current_image_ = 0;
    vk::Result swapchain_state_ = vk::Result::eSuccess;

public:
    struct RenderOptions
    {
        int  shadow_map_size            = 2048u; // TODO: take it from console setting
        bool shader__dump_source        = true;
        bool shader__disassemble        = false;
    } o;
};

extern FrontEnd frontend;

#endif // FRONTEND_RENDER_H_
