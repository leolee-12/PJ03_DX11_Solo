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

	/* -------- Spawn/Wander 쿼리 (S1 추가) --------
   호출자 (CSpawn_Manager 등) 가 m_iCurrentCellIndex 와 무관하게 사용할 수 있도록
   모두 const 메서드. iAreaMask 는 1차 no-op. */
	_bool Project_PointToNavigation(
		const _float3& vWorldPos, _float fSearchRadius, _uint iAreaMask,
		_float3* pOutNavPos, _uint* pOutCellIndex) const;

	_bool Is_Reachable(
		_uint iStartCellIndex, _uint iGoalCellIndex, _uint iAreaMask) const;

	_bool Find_RandomPoint_InRect(
		const _float3& vCenter, const _float2& vSize, _float fRotationY,
		_float fProjectRadius, _uint iAreaMask,
		_float3* pOutNavPos, _uint* pOutCellIndex) const;

	_bool Find_RandomReachablePoint_InRadius(
		const _float3& vOrigin, _uint iOriginCellIndex,
		_float fRadius, _uint iAreaMask,
		_float3* pOutNavPos, _uint* pOutCellIndex) const;

#ifdef _DEBUG
public:
	HRESULT Render() override;
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