#pragma once
#include "Actor.h"
#include "Body.h"

NS_BEGIN(Game_PKM)

class CInteraction_BallHit;

class CActor_CaptureTarget final : public CActor
{
public:
    struct ACTOR_CAPTURE_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        WNameID            strBodyProtoTag = { 0 };
        CBody::BODY_DESC* pBodyDesc = { nullptr };
        _uint              iBodyProtoLevel = ETOUI(LEVEL::STATIC);
        _uint              iComponentLevel = ETOUI(LEVEL::GAMEPLAY);

        _uint              iSpeciesID = { 0 };   // 표시·결과용
        _uint              iLevel = { 1 };
        _uint              iInitialBallItemID = { 0 }; // 처음 던지는 볼 종류 (테스트용)
    };

private:
    CActor_CaptureTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CActor_CaptureTarget(const CActor_CaptureTarget& Prototype);
    virtual ~CActor_CaptureTarget() = default;

public:
    virtual _string Get_TypeName() const override { return "CaptureTargetActor"; }

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    CInteraction_BallHit* m_pBallHit = { nullptr };

private:
    HRESULT Ready_Components(const ACTOR_CAPTURE_DESC* pDesc);
    HRESULT Ready_PartObjects(const ACTOR_CAPTURE_DESC* pDesc);
    void    Cache_Members();

public:
    static CActor_CaptureTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;

private:
    virtual void Free() override;
};

NS_END