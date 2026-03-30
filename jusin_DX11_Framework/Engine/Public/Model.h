#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _cmatrix PreTransformMatrix);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	size_t Get_NumMeshes() const { return m_iNumMeshes; }

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	HRESULT Render(_uint iMeshIndex);
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eType, _uint iIndex);

private:
	const aiScene* m_pAIScene = { nullptr };
	Importer m_Importer = {};
	const _string m_strModelFilePath = {};
	MODEL m_eType = { MODEL::END };
	_float4x4 m_PreTransformMatrix = {};

	size_t m_iNumMeshes = {};
	vector<class CMesh*> m_Meshes;
	size_t m_iNumMaterials = {};
	vector<class CMaterial*> m_Materials;
private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials();

public:
	static CModel* XM_CALLCONV Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END