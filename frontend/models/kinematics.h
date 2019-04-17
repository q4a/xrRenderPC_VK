
#include "Include/xrRender/Kinematics.h"

#include "frontend/models/hierrarhy_visual.h"

class Kinematics
    : public HierrarhyVisual
    , public IKinematics
{
public:
    // Interface implementation
    void Bone_Calculate(CBoneData* bd, Fmatrix* parent) {}
    void Bone_GetAnimPos(Fmatrix& pos, u16 id, u8 channel_mask, bool ignore_callbacks) {}

    bool PickBone(const Fmatrix& parent_xform, pick_result& r, float dist, const Fvector& start,
        const Fvector& dir, u16 bone_id) { return false; }
    void EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id) {}

    u16 LL_BoneID(LPCSTR B) { return 0; }
    u16 LL_BoneID(const shared_str& B) { return 0; }
    LPCSTR LL_BoneName_dbg(u16 ID) { return ""; }

    CInifile* LL_UserData() { return nullptr; }
    accel* LL_Bones() { return nullptr; }

    CBoneInstance& LL_GetBoneInstance(u16 bone_id) { return CBoneInstance{}; }
    CBoneData& LL_GetData(u16 bone_id) { return CBoneData{ 0 }; }

    const IBoneData& GetBoneData(u16 bone_id) const { return CBoneData{ 0 }; }

    u16 LL_BoneCount() const { return 0; }
    u16 LL_VisibleBoneCount() { return 0; }

    Fmatrix& LL_GetTransform(u16 bone_id) { return Fmatrix{}; }
    const Fmatrix& LL_GetTransform(u16 bone_id) const { return Fmatrix{}; }

    Fmatrix& LL_GetTransform_R(u16 bone_id) { return Fmatrix{}; }
    Fobb& LL_GetBox(u16 bone_id) { return Fobb{}; }
    const Fbox& GetBox() const { return Fbox{}; }
    void LL_GetBindTransform(xr_vector<Fmatrix>& matrices) {}
    int LL_GetBoneGroups(xr_vector<xr_vector<u16>>& groups) { return 0; }

    u16 LL_GetBoneRoot() { return 0; }
    void LL_SetBoneRoot(u16 bone_id) {}

    BOOL LL_GetBoneVisible(u16 bone_id) { return false; }
    void LL_SetBoneVisible(u16 bone_id, BOOL val, BOOL bRecursive) {}
    u64 LL_GetBonesVisible() { return 0; }
    void LL_SetBonesVisible(u64 mask) {}

    void LL_AddTransformToBone(KinematicsABT::additional_bone_transform& offset) {}
    void LL_ClearAdditionalTransform(u16 bone_id) {}

    void CalculateBones(BOOL bForceExact = FALSE) {}
    void CalculateBones_Invalidate() {}
    void Callback(UpdateCallback C, void* Param) {}

    void SetUpdateCallback(UpdateCallback pCallback) {}
    void SetUpdateCallbackParam(void* pCallbackParam) {}

    UpdateCallback GetUpdateCallback() { return {}; }
    void* GetUpdateCallbackParam() { return nullptr; }

    IRenderVisual* dcast_RenderVisual() { return nullptr; }
    IKinematicsAnimated* dcast_PKinematicsAnimated() { return nullptr; }

#ifdef DEBUG
    void DebugRender(Fmatrix& XFORM) {}
    shared_str getDebugName() { return shared_str{}; }
#endif
};
