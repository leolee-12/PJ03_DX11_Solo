#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNaviDataFile);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

private:
	const _tchar* m_pNaviDataFile = {};
	vector<class CCell*> m_Cells;
	_uint m_iCurrentCellIndex = {};

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNaviDataFile);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END