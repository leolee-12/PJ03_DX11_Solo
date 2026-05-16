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