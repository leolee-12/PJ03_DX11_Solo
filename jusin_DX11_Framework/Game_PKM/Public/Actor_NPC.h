#pragma once
#include "Actor.h"
#include "Body.h"

NS_BEGIN(Game_PKM)

class CInteraction_Dialogue;

class CActor_NPC final : public CActor
{
public:
	struct ACTOR_NPC_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		WNameID strBodyProtoTag = { 0 };
		CBody::BODY_DESC* pBodyDesc = { nullptr };
		_uint iBodyProtoLevel = ETOUI(LEVEL::STATIC);
		_wstring strDialogueKey;
		_uint iComponentLevel = ETOUI(LEVEL::GAMEPLAY);

		_bool   bIsTrainer = { false };
		_uint   iSpawnRectID = { 0 };
		_bool   bApplyInitialRotation = { false };
		_float  fInitialRotationY = { 0.f };   // 라디안
	};

private:
	CActor_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_NPC(const CActor_NPC& Prototype);
	virtual ~CActor_NPC() = default;

public:
	virtual _string Get_TypeName() const override { return "NPCActor"; }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void XM_CALLCONV Face_To(_fvector vTargetPos);

private:
	CInteraction_Dialogue* m_pDialogue = { nullptr };
	_uint m_iSpawnRectID = { 0 };

	_bool   m_bFaceTurnActive = { false };
	_float3 m_vFaceTurnTarget = {};
	_float  m_fFaceTurnRadiansPerSec = { XMConvertToRadians(720.f) };

private:
	HRESULT Ready_Components(const ACTOR_NPC_DESC* pDesc);
	HRESULT Ready_PartObjects(const ACTOR_NPC_DESC* pDesc);
	void Cache_Members();

	void Update_FaceTurn(_float fTimeDelta);

public:
	static CActor_NPC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END