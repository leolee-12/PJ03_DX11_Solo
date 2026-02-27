//#pragma once
//
//#include "Base.h"
//
//BEGIN(Engine)
//
//class ENGINE_DLL CCell final : public CBase
//{
//public:
//	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
//	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
//private:
//	CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
//	virtual ~CCell() = default;
//
//public:
//	const _float3* Get_Point(POINT ePoint) const {
//		return &m_vPoints[ePoint];
//	}
//
//public:
//	HRESULT Initialize(const _float3* pPoints, _uint iIndex);
//	_bool Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint);
//	_bool isIn(_fvector vPosition, _fmatrix WorldMatrix, _int* pNeighborIndex, _float3* vLine );
//	_bool isIn(_fvector vPosition, _uint& iIndex);
//	void Update(_fmatrix WorldMatrix);
//
//	void	Make_Line();
//	_uint	Get_Index() { return m_iIndex; }
//	void	Set_Index(_uint iIndex) { m_iIndex = iIndex; }
//	void	SetUp_Neighbor(LINE eLine, CCell* pNeighborCell) {
//		m_iNeighbors[eLine] = pNeighborCell->m_iIndex;
//	}
//
//	void	SetUp_Neighbor(LINE eLine, _uint iNeighborIndex) {
//		m_iNeighbors[eLine] = iNeighborIndex;
//	}
//
//#ifdef _DEBUG
//public:
//	HRESULT Render(shared_ptr<class CShader> pShader);
//#endif
//
//private:
//	ComPtr<ID3D11Device> m_pDevice = { nullptr };
//	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
//
//	_float3					m_vPoints[POINT_END] = {};
//	_int					m_iNeighbors[LINE_END] = { -1, -1, -1 };
//	_float3					m_vLine[LINE_END];
//	_float3					m_vNormals[LINE_END] = {};
//	_uint					m_iIndex = { 0 };
//
//#ifdef _DEBUG
//private:
//	 shared_ptr<class CVIBuffer_Cell> m_pVIBuffer = { nullptr };
//#endif
//
//public:
//	HRESULT		 Bake_Binary(ofstream& fout);
//	HRESULT		 Load_Binary(ifstream& fin);
//
//
//public:
//	static CCell* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, _uint iIndex);
//	virtual void Free() override;
//};
//
//END