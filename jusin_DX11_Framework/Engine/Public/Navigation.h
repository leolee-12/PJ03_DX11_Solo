#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	struct NAVIGATION_DESC
	{
		_int iCurrentCellIndex;
		const _float4x4* pParentMatrix = { nullptr };
	};

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNaviDataFile);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	HRESULT SetUp_Neighbors();
	_bool XM_CALLCONV Is_Move(_fvector vResultPos);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif
	
private:
	const _tchar* m_pNaviDataFile = {};
	vector<class CCell*> m_Cells;
	_uint m_iCurrentCellIndex = {};
	static const _float4x4* m_pParentMatrixPtr;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNaviDataFile);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END