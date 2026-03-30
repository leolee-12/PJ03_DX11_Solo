#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _cmatrix PreTransformMatrix);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }
	
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

private:
	const aiMesh* m_pAIMesh = { nullptr };
	_uint m_iMaterialIndex = {};
	_float4x4 m_PreTransformMatrix = {};

public:
	static CMesh* XM_CALLCONV Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _cmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END