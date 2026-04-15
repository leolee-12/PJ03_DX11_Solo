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
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNaviFilePath, const _tchar* pNeighborFilePath);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	_vector Get_CellPos();

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	HRESULT SetUp_Neighbors();
	HRESULT SetUp_NeighborsFromFile();
	_bool XM_CALLCONV Is_Move(_fvector vResultPos);
	_vector Compute_OnNavigation(const class CTransform* pTargetTransform);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif
	
private:
	const _tchar* m_pNaviFilePath = {};
	const _tchar* m_pNeighborFilePath = {};
	vector<class CCell*> m_Cells;
	_uint m_iCurrentCellIndex = {};
	static const _float4x4* m_pParentMatrixPtr;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNaviFilePath, const _tchar* pNeighborFilePath);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END