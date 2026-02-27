#pragma once
#include "VIBuffer_Model_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Inst final : public CVIBuffer_Model_Instancing
{
public:
	typedef struct tagMesh_Instance_Desc
	{
		string					strName;	
		vector<_float4x4>		vecInstanceVertex;

	}MESH_INSTANCE_DESC;

private:
	CModel_Inst(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CModel_Inst(const CModel_Inst& rhs);
	virtual	~CModel_Inst() = default;

public:
	virtual	HRESULT	Initialize_Prototype();
	virtual	HRESULT	Initialize(void* pArg);
	virtual void	Update(_cref_time fTimeDelta);
	virtual HRESULT	Render(_uint iMeshIndx);

public:
	HRESULT	Bind_ShaderResources(class CShader* pShader, const _char* pName, _uint iMeshIndex, TEXTURETYPE eType);

public:
	_uint	Get_MeshesNum() { return m_iMeshesNum; }

private:
	string	m_strModelFilePath = {};
	_uint	m_iMaterialIndex = { 0 };

private:
	_uint					m_iMeshesNum = { 0 };
	vector<class shared_ptr<CMeshComp>>			m_vecMesh;

	_uint					m_iMaterialsNum = { 0 };
	vector<MATERIAL_DESC>	m_vecMaterial;

private:
	_float4x4				m_matPivot;

private:
	HRESULT	Ready_Meshes(const class FModelData* ModelData, const string& strModelFilePath);
	HRESULT	Ready_Materials(const class FModelData* ModelData, const string& strModelFilePath);

public:
	static	shared_ptr<CModel_Inst> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	shared_ptr<CComponent> Clone(void* pArg) override;
	virtual	void	Free() override;
};

END