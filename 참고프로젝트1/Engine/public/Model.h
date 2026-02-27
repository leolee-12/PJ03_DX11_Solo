#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
protected:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	const _float4x4* Get_SocketBoneMatrix_Ptr(const _char* pBoneName) const;

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_ShaderResource(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iIndex);
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);

public:
	void Play_Animation(_float fTimeDelta);
	void Set_Animation(_uint iIndex, _bool isLoop = false);
	_bool is_AnimFinished() const {
		return m_isAnimFinished;
	}


private:
	const aiScene*			m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer = {};
	MODELTYPE				m_eType = { };
	_float4x4				m_PreTransformMatrix = {};

private:
	_uint					m_iNumMeshes = {};
	vector<class CMesh*>	m_Meshes;

private:
	_uint						m_iNumMaterials = {};
	vector<class CMaterial*>	m_Materials;

private:
	vector<class CBone*>		m_Bones;	

private:
	_bool						m_isAnimLoop = { false };
	_bool						m_isAnimFinished = { false };
	_uint						m_iCurrentAnimIndex = {};
	_uint						m_iNumAnimations = {};
	vector<class CAnimation*>	m_Animations;


private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END

