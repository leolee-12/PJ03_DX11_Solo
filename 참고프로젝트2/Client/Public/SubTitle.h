#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CSubTitle final : public CGameObject
{
protected:
	CSubTitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSubTitle(const CSubTitle& Prototype);
	virtual ~CSubTitle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	HRESULT Initialize_SubTitle(const _wstring& strText, _float fDuration);

private:
	_wstring m_strText;
	_float m_fDuration = 0.f;

public:
	static CSubTitle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END