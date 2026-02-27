#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* VIBuffer_Rect랑 크게 다를게 없다. */

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	HRESULT Initialize_Prototype(_bool isAnim, const MeshData& meshData, const vector<class CBone*>& m_Bones, _fmatrix PreTransformMatrix);

public:
	void SetUp_Matrices(_float4x4* pMatrices, const vector<class CBone*>& Bones);
private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iMaterialIndex = {};
	/* 이 메시에게 영향을 주는 뼈의 갯수 .*/
	_uint				m_iNumBones = {};

	/* 이 메시에게 영향을 주는 뼈의 인덱스를 저장한다. (전체뼈기준) */
	vector<_uint>		m_BoneIndices;
	vector<_float4x4>	m_OffsetMatrices;

private:
	void* Create_NonAnimVertices(const vector<VTXMESH>& Vertices, _fmatrix PreTransformMatrix);
	void* Create_AnimVertices(const vector<VTXANIMMESH>& animVertices, const vector<BoneData>& bones, const vector<class CBone*>& m_Bones);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _bool isAnim, const MeshData& meshData, const vector<class CBone*>& m_Bones, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END