#pragma once
#include "Game_PKM_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Game_PKM)

class CPlayer final : public CContainerObject
{
public:
	enum PLAYER_STATE {
		IDLE = 0x00000001,
		RUN = 0x00000002,
		ATTACK = 0x00000004,
		JUMP = 0x00000008,
	};

#define NOT_RUN PLAYER_STATE::IDLE | PLAYER_STATE::JUMP
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCollider* m_pColliderCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	_uint m_iState = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END