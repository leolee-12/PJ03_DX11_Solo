#pragma once
#include "Base.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CModel;
NS_END

NS_BEGIN(Editor)
class CEditInstance;
class CVP_CoordMapper;

class CVP_PickingCtrl final : public CBase
{
private:
    CVP_PickingCtrl();
    virtual ~CVP_PickingCtrl() = default;

public:
    HRESULT Initialize(CVP_CoordMapper* pMapper);

    void Handle_DebugPicking();
    void Handle_ViewportClick();

    _bool Has_LastHit() const { return m_bHasLastHit; }
    const _float3& Get_LastObjectPos() const { return m_vLastObjectPos; }
    const _float3& Get_LastLocalHitPos() const { return m_vLastLocalHitPos; }
    const _float3& Get_LastWorldHitPos() const { return m_vLastWorldHitPos; }
    const _string& Get_PickDebug() const { return m_strPickDebug; }
    const _string& Get_PickTarget() const { return m_strPickTarget; }

private:
    CGameInstance* m_pGameInstance = { nullptr };
    CEditInstance* m_pEditInstance = { nullptr };
    CVP_CoordMapper* m_pMapper = { nullptr };

    _bool m_bHasLastHit = { false };
    _float3 m_vLastObjectPos = {};
    _float3 m_vLastLocalHitPos = {};
    _float3 m_vLastWorldHitPos = {};
    _string m_strPickDebug = { "No Pick" };
    _string m_strPickTarget = { "Target : None" };
    CGameObject* m_pLastPickedObject = { nullptr };

private:
    _bool Build_MouseRay(_float3* pOutOrigin, _float3* pOutDir) const;
    _bool Pick_ModelObject(CGameObject* pObj, CModel* pModel, _fvector vRayOrigin,
        _fvector vRayDir, _float3* pOutLocalHitPos, _float3* pOutWorldHitPos) const;

    void Place_ObjectAtHit(const _float3& vHitPos);
    void Pick_SelectObject();

public:
    static CVP_PickingCtrl* Create(CVP_CoordMapper* pMapper);

private:
    virtual void Free() override;
};

NS_END