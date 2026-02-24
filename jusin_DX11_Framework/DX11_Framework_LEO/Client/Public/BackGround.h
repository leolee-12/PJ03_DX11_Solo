#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CBackGround final : public CGameObject
{
protected:
	CBackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CBackGround(const CBackGround& Prototype);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

public:
	static CBackGround*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBackGround*	Clone(void* pArg) override;

protected:
	virtual void			Free() override;
};

NS_END