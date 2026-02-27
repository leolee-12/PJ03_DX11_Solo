#pragma once

#include "VIBuffer.h"
#include "Model.h"
#include "Mesh_Data.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
protected:
	CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
#ifdef _DEBUG
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType,const aiMesh* pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*> Bones);
#endif
	virtual HRESULT Initialize_Prototype(MESH_DATA MeshData);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CBone*> Bones);

private:
	MODELTYPE		m_eModelType = { MODELTYPE::NONANIM };
	char				m_szName[MAX_PATH];
	_uint				m_iMaterialIndex = { 0 };

	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;

	vector<_float4x4>	m_OffsetMatrices;
public:
	MODELTYPE		Get_ModelType() { return m_eModelType; }
	const _char* Get_Name() const {
		return m_szName;
	}
private:
#ifdef _DEBUG
	HRESULT Ready_Vertices_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_Vertices_Anim(const aiMesh* pAIMesh, const vector<class CBone*> Bones);
#endif
	HRESULT Ready_Vertices_NonAnim(MESH_DATA MeshData);
	HRESULT Ready_Vertices_Anim(MESH_DATA MeshData);

public:
#ifdef _DEBUG
	static CMesh* Create(MODELTYPE eModelType, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const aiMesh* pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*> Bones);
#endif
	static CMesh* Create(MESH_DATA MeshData, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext );
	virtual shared_ptr<CComponent> Clone(void* pArg);
	virtual void Free() override;
};

END