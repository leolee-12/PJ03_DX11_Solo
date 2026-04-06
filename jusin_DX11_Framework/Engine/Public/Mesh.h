#pragma once
#include "VIBuffer.h"
#include "Model.h"

NS_BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, class CModel* pModel, const aiMesh* pAIMesh, _cmatrix PreTransformMatrix);
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const WMODEL_MESH& tMeshData);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }

	const vector<XMFLOAT3>& Get_Positions() const { return m_vecPositions; }
	const vector<_uint>& Get_Indices() const { return m_vecIndices; }
	const BoundingBox& Get_LocalAABB() const { return m_tLocalAABB; }
	
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstName, vector<class CBone*>& Bones);

private:
	_char m_szName[MAX_PATH] = {};
	MODEL m_eType = { MODEL::END };
	const aiMesh* m_pAIMesh = { nullptr };
	CModel* m_pModel = { nullptr };
	
	_uint m_iMaterialIndex = {};

	_uint m_iNumBones = {};
	_float4x4 m_PreTransformMatrix = {};
	_float4x4 m_BoneMatrices[g_iNumMeshBones] = {};
	vector<_uint> m_BoneIndices;
	vector<_float4x4> m_OffsetMatrices;

	WMODEL_MESH m_tMesh;

	vector<XMFLOAT3> m_vecPositions;
	vector<_uint> m_vecIndices;
	BoundingBox m_tLocalAABB = {};

public:
	static CMesh* XM_CALLCONV Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, class CModel* pModel, const aiMesh* pAIMesh, _cmatrix PreTransformMatrix);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const WMODEL_MESH& tMeshData);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END