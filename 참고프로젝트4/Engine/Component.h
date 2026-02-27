#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase, public enable_shared_from_this<CComponent>
{
	INFO_CLASS(CComponent, CBase)

protected:
	CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;
		
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

#ifdef _DEBUG
	virtual HRESULT Render() {
		return S_OK;
	}
#endif

protected:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

protected:
	_bool					m_isCloned = { false };
	weak_ptr<class CGameObject>			m_pOwner;

public:
	GETSET_EX2(weak_ptr<CGameObject>,m_pOwner,Owner,GET,SET)

public:
	virtual void Write_Json(json & Out_Json) {};
	virtual void Load_Json(const json & In_Json) {};

public:
	virtual shared_ptr<CComponent> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END