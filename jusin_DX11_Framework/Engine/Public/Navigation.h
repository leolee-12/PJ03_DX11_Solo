#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	struct NAVIGATION_DESC
	{
		_int iCurrentCellIndex;
	};

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNaviFilePath, const _tchar* pNeighborFilePath);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	void Set_CurrentCellIndex(_int iCellIdx);
	_int Get_CurrentCellIndex() const { return m_iCurrentCellIndex; }
	_vector Get_CellPos();

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	HRESULT SetUp_Neighbors();
	HRESULT SetUp_NeighborsFromFile();
	_bool XM_CALLCONV Is_Move(_fvector vResultPos);
	_vector Compute_OnNavigation(const class CTransform* pTargetTransform);
	_vector XM_CALLCONV Compute_Height(_fvector vPos) const;
	_int XM_CALLCONV Find_CellIndex_ByPos(_fvector vWorldPos) const;
	_vector XM_CALLCONV Compute_SlidePos(_fvector vCurPos, _fvector vDesiredPos);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif
	
private:
	const _tchar* m_pNaviFilePath = {};
	const _tchar* m_pNeighborFilePath = {};
	vector<class CCell*> m_Cells;
	_uint m_iCurrentCellIndex = {};

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNaviFilePath, const _tchar* pNeighborFilePath = nullptr);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END