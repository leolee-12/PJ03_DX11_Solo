#pragma once

#include "Base.h"

/* 객체의 기능을 수행하는 컴포넌트들의 부모 클래스이다. */

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_float fTimeDetla) { return S_OK; }

protected:
	 ID3D11Device*			m_pDevice = { nullptr };
	 ID3D11DeviceContext*	m_pContext = { nullptr };
	_bool					m_isCloned = { false };
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END