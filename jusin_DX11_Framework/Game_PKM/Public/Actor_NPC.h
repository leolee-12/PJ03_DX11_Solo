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
		_uint iBodyProtoLevel = ETOUI(LEVEL::STATIC);          // Body Proto레벨
		_wstring strDialogueKey;
		_uint iComponentLevel = ETOUI(LEVEL::GAMEPLAY);        // Interaction 등 컴포넌트 Proto레벨

		// S4 추가 - 트레이너 스폰 페이로드 (SpawnManager 가 채움). 기존 비-트레이너 NPC 는 default 유지.
		_bool   bIsTrainer = { false };
		_uint   iSpawnRectID = { 0 };
		_float  fInitialRotationY = { 0.f };   // 라디안. bIsTrainer 가 true 인 경우에만 Initialize 시 적용
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

private:
	CInteraction_Dialogue* m_pDialogue = { nullptr };
	
	// S4 추가 - 트레이너 페이로드 캐싱
	_uint m_iSpawnRectID = { 0 };

private:
	HRESULT Ready_Components(const ACTOR_NPC_DESC* pDesc);
	HRESULT Ready_PartObjects(const ACTOR_NPC_DESC* pDesc);
	void Cache_Members();

public:
	static CActor_NPC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END