#pragma once
#include "Game_PKM_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Game_PKM)
class CActor;

class CPlayer_LGPE final : public CContainerObject
{
private:
	CPlayer_LGPE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_LGPE(const CPlayer_LGPE& Prototype);
	virtual ~CPlayer_LGPE() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Clear_TouchSet();

private:
	_uint m_iState = {};
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CActor* m_pCurrentInteractTarget = { nullptr };	// weak
	unordered_set<CActor*> m_PrevTouchSet;			// 직전 프레임 overlap 액터 (weak)
	WNameID m_strBodyModelProtoTag = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

	_vector Read_MoveInput() const;
	void Update_AnimState(_bool bHasInput);

	void Update_Interaction(_float fTimeDelta);
	CActor* Find_InteractionCandidate() const;
	void Try_Talk();

	void Update_TouchTriggers();
	void Fire_Touch(CActor* pActor);

public:
	static CPlayer_LGPE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END