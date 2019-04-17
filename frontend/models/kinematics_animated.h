/*
*/

#include "Include/xrRender/KinematicsAnimated.h"

#include "frontend/models/kinematics.h"


class KinematicsAnimated
    : public Kinematics
    , public IKinematicsAnimated
{
public:
    virtual void OnCalculateBones() {}

#ifdef DEBUG
    std::pair<LPCSTR, LPCSTR> LL_MotionDefName_dbg(MotionID ID) { return std::make_pair("", ""); }
    void LL_DumpBlends_dbg() {}
#endif

    u32 LL_PartBlendsCount(u32 bone_part_id) { return 0; }
    CBlend* LL_PartBlend(u32 bone_part_id, u32 n) { return nullptr; }
    void LL_IterateBlends(IterateBlendsCallback& callback) {}

    u16 LL_MotionsSlotCount() { return 0; }
    const shared_motions& LL_MotionsSlot(u16 idx) { return shared_motions{}; }

    CMotionDef* LL_GetMotionDef(MotionID id) { return nullptr; }
    CMotion* LL_GetRootMotion(MotionID id) { return nullptr; }
    CMotion* LL_GetMotion(MotionID id, u16 bone_id) { return nullptr; }
    void LL_BuldBoneMatrixDequatize(const CBoneData* bd, u8 channel_mask, SKeyTable& keys) {}
    void LL_BoneMatrixBuild(CBoneInstance& bi, const Fmatrix* parent, const SKeyTable& keys) {}

    void LL_AddTransformToBone(KinematicsABT::additional_bone_transform& offset) {}
    void LL_ClearAdditionalTransform(u16 bone_id) {}

    IBlendDestroyCallback* GetBlendDestroyCallback() { return nullptr; }
    void SetBlendDestroyCallback(IBlendDestroyCallback* cb) {}
    void SetUpdateTracksCalback(IUpdateTracksCallback* callback) {}
    IUpdateTracksCallback* GetUpdateTracksCalback() { return nullptr; }

    MotionID LL_MotionID(LPCSTR B) { return MotionID{}; }
    u16 LL_PartID(LPCSTR B) { return 0; }

    CBlend* LL_PlayCycle(u16 partition, MotionID motion, BOOL bMixing, float blendAccrue, float blendFalloff,
        float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam, u8 channel = 0) { return nullptr; }
    CBlend* LL_PlayCycle(
        u16 partition, MotionID motion, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel = 0) { return nullptr; }
    void LL_CloseCycle(u16 partition, u8 mask_channel = (1 << 0)) {}
    void LL_SetChannelFactor(u16 channel, float factor) {}

    void UpdateTracks() {}
    void LL_UpdateTracks(float dt, bool b_force, bool leave_blends) {} // Update motions

    MotionID ID_Cycle(LPCSTR N) { return MotionID{}; }
    MotionID ID_Cycle_Safe(LPCSTR N) { return MotionID{}; }
    MotionID ID_Cycle(shared_str N) { return MotionID{}; }
    MotionID ID_Cycle_Safe(shared_str N) { return MotionID{}; }
    CBlend* PlayCycle(
        LPCSTR N, BOOL bMixIn = TRUE, PlayCallback Callback = nullptr, LPVOID CallbackParam = nullptr, u8 channel = 0) { return nullptr; }
    CBlend* PlayCycle(
        MotionID M, BOOL bMixIn = TRUE, PlayCallback Callback = nullptr, LPVOID CallbackParam = nullptr, u8 channel = 0) { return nullptr; }
    CBlend* PlayCycle(u16 partition, MotionID M, BOOL bMixIn = TRUE, PlayCallback Callback = nullptr,
        LPVOID CallbackParam = nullptr, u8 channel = 0) { return nullptr; }

    MotionID ID_FX(LPCSTR N) { return MotionID{}; }
    MotionID ID_FX_Safe(LPCSTR N) { return MotionID{}; }
    CBlend* PlayFX(LPCSTR N, float power_scale) { return nullptr; }
    CBlend* PlayFX(MotionID M, float power_scale) { return nullptr; }

    const CPartition& partitions() const { return CPartition{}; }

    IRenderVisual *dcast_RenderVisual() final;
    IKinematics *dcast_PKinematics() final;

    float get_animation_length(MotionID motion_ID) { return 0.0f; }
};
