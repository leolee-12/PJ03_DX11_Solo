#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones);

private:
	_char			m_szName[MAX_PATH] = {};
	/* 이 메시가 이용해야할  CMaterial을 의미한다. */
	_uint			m_iMaterialIndex = {};
	_uint			m_iNumBones = { };

	/* 이 메시에 영향을 주는 뼈들의 인덱스(전체 모델 기준(CModel::m_Bones)) 를 모아둔다. */
	vector<_uint>	m_BoneIndices;
	_float4x4		m_BoneMatrices[g_iMaxNumBones] = {};
	vector<_float4x4>	m_OffsetMatrices;

public:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);



public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END