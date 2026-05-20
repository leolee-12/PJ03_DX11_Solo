#pragma once
#include "Actor.h"
#include "Body.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Game_PKM)
class CInteraction_BallHit;
class CEffect;

class CActor_CaptureTarget final : public CActor
{
public:
	struct ACTOR_CAPTURE_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		WNameID				strBodyProtoTag = { 0 };
		CBody::BODY_DESC*	pBodyDesc = { nullptr };
		_uint				iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		_uint				iComponentLevel = ETOUI(LEVEL::GAMEPLAY);

		_uint				iSpeciesID = { 0 };   // 표시·결과용
		_uint				iLevel = { 1 };
		_uint				iInitialBallItemID = { 0 };
		_bool				bCaughtBefore = { false };
	};

private:
	CActor_CaptureTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_CaptureTarget(const CActor_CaptureTarget& Prototype);
	virtual ~CActor_CaptureTarget() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _string Get_TypeName() const override { return "CaptureTargetActor"; }
	_uint Get_SpeciesID() const { return m_iSpeciesID; }
	_uint Get_Level() const { return m_iLevel; }
	_bool Is_CaughtBefore() const { return m_bCaughtBefore; }
	CCollider* Get_Collider() const { return m_pColliderCom; }
	_float  Get_CaptureRadius() const { return m_fCaptureRadius; }
	_float3 Get_CaptureCenter() const;

	void    Begin_Absorb();
	void    Begin_Appear();
	_bool   Is_Absorbing() const { return m_bAbsorbing; }

private:
	CInteraction_BallHit* m_pBallHit = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	_uint m_iSpeciesID = { 0 };
	_uint m_iLevel = { 1 };
	_bool m_bCaughtBefore = { false };

	_float  m_fCaptureRadius = { 0.6f };
	_float3 m_vCaptureCenter = { 0.f, 0.5f, 0.f };

	_bool   m_bAbsorbing = { false };
	_bool   m_bAbsorbReverse = { false };
	_float  m_fAbsorbElapsed = { 0.f };
	_float  m_fAbsorbDuration = { 0.8f };
	CEffect* m_pAbsorbEffect = { nullptr };

	_bool   m_bBasisCached = { false };
	_float3 m_vRightUnit = { 1.f, 0.f, 0.f };
	_float3 m_vUpUnit = { 0.f, 1.f, 0.f };
	_float3 m_vLookUnit = { 0.f, 0.f, 1.f };

private:
	HRESULT Ready_Components(const ACTOR_CAPTURE_DESC* pDesc);
	HRESULT Ready_PartObjects(const ACTOR_CAPTURE_DESC* pDesc);
	void    Cache_Members();
	void    Cache_BasisIfNeeded();

public:
	static CActor_CaptureTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END
