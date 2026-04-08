#pragma once
#include "Game_PKM_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Game_PKM)

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

private:
	_uint m_iState = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer_LGPE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END