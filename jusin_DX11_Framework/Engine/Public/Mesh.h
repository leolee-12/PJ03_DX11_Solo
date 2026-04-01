#pragma once
#include "VIBuffer.h"
#include "Model.h"

NS_BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, class CModel* pModel, const aiMesh* pAIMesh, _cmatrix PreTransformMatrix);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }
	
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

private:
	HRESULT Ready_NonAnimMesh();
	HRESULT Ready_AnimMesh(CModel* pModel);

public:
	static CMesh* XM_CALLCONV Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, class CModel* pModel, const aiMesh* pAIMesh, _cmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END