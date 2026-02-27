//#pragma once
//
//#include "Component.h"
//
//BEGIN(Engine)
//
//class ENGINE_DLL CNavigation final : public CComponent
//{
//public:
//	typedef struct tagNaviDesc
//	{
//		_int		iCurrentIndex;
//	}NAVI_DESC;
//private:
//	CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
//	CNavigation(const CNavigation& rhs);
//	virtual ~CNavigation() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype(_uint iNumLevels, const wstring & strNavigationFilePath);
//	virtual HRESULT Initialize(void* pArg) override;
//	virtual HRESULT Render();
//
//public:
//	void Update(_fmatrix WorldMatrix);
//	_bool isMove(_fvector vPosition, _float3 * vLine);
//	_float Get_HeightOnNavigation(_fvector vPosition);
//
//	HRESULT				  Bake_Cells(vector<array<_float3, 3>>*pCellPoints);
//	vector<class CCell*>* Get_Cells(_uint iLevelIndex) { return &m_Cells[iLevelIndex]; }
//	HRESULT				  Make_Neighbors();
//	void				  Set_CurrentIndex(_int iIndex) { m_iCurrentIndex = iIndex; }
//	_int				  Get_CurrentIndex() { return m_iCurrentIndex; }
//
//	void				  Reset_Cells();
//private:
//	static vector<class CCell*>* m_Cells;
//	static _float4x4				m_WorldMatrix;
//
//	_uint							m_iCurrentLevel = { 1 };
//	_uint							m_iNumLevels;
//	_int							m_iCurrentIndex = { -1 };
//
//#ifdef _DEBUG
//private:
//	shared_ptr<class CShader> m_pShader = { nullptr };
//public:
//	static void Copy_Cell_LevelToLevel(const _uint & iDestLevel, const _uint & iSourLevel);
//
//	HRESULT		 Bake_Binary(ofstream & fout, _uint iLevelIndex, _uint iToolLevelIndex);
//#endif
//
//public:
//	virtual void Write_Json(json & Out_Json);
//	virtual void Load_Json(const json & In_Json);
//
//	HRESULT		 Load_Binary(ifstream & fin);
//public:
//	static shared_ptr<CNavigation> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumLevels, const wstring & strNavigationFilePath);
//	virtual shared_ptr<CComponent> Clone(void* pArg) override;
//	virtual void Free() override;
//};
//
//END