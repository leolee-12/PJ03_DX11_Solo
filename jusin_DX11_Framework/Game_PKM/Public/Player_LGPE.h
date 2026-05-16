#pragma once
#include "Game_PKM_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Game_PKM)
class CActor;

class CPlayer_LGPE final : public CContainerObject
{
private:
	enum HERO_ANIM
	{
		IDLE = 17,
		RUN = 76
	};

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

private:
	_uint m_iState = {};
	CNavigation* m_pNavigationCom = { nullptr };
	CActor* m_pCurrentInteractTarget = { nullptr };   // weak — 매 프레임 후보 갱신

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

	_vector Read_MoveInput() const;
	void Update_AnimState(_bool bHasInput);

	void Update_Interaction(_float fTimeDelta);
	CActor* Find_InteractionCandidate() const;
	void Try_Talk();

public:
	static CPlayer_LGPE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END