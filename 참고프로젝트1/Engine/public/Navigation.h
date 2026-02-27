#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc 
	{
		_int			iCurrentCellIndex = {-1};
	}NAVIGATION_DESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;
public:
	void Update(const _float4x4* pParentMatrix) {
		m_pParentMatrix = pParentMatrix;
	}
	_bool isMove(_fvector vResultPos);
	_vector SetUp_OnNavigation(_fvector vWorldPos);



#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	_int					m_iCurrentCellIndex = { -1 };
	vector<class CCell*>	m_Cells;
	static const _float4x4*	m_pParentMatrix;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

private:
	void SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END