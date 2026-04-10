#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext*, const _char* pModelFilePath = nullptr);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	size_t Get_NumMeshes() const { return m_iNumMeshes; }
	class CMesh* Get_Mesh(_uint iIndex) const
	{
		if (iIndex >= m_Meshes.size())
			return nullptr;

		return m_Meshes[iIndex];
	}

	_int Get_BoneIndex(const _char* pBoneName);
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;
	void Set_AnimationIndex(_uint iIndex, _bool isLoop = false) { m_iCurrentAnimationIndex = iIndex; m_isAnimLoop = isLoop; }

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	_bool Play_Animation(_float fTimeDelta);
	HRESULT Render(_uint iMeshIndex);
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstName, _uint iMeshIndex, TEXTURE_TYPE eType, _uint iIndex);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstName, _uint iMeshIndex);

private:
	const _string m_pModelFilePath = {};
	MODEL m_eType = { MODEL::END };
	_float4x4 m_PreTransformMatrix = {};

	size_t m_iNumMeshes = {};
	vector<class CMesh*> m_Meshes;

	size_t m_iNumMaterials = {};
	vector<class CMaterial*> m_Materials;
	vector<class CBone*> m_Bones;
	
	_uint m_iCurrentAnimationIndex = {};
	_uint m_iNumAnimations = {};
	_bool m_isAnimLoop = { false };
	vector<class CAnimation*> m_Animations;

private:
	HRESULT Ready_Meshes(FILE* fp, _uint iNumMeshes);
	HRESULT Ready_Materials(FILE* fp, _uint iNumMaterials);
	HRESULT Ready_Bones(FILE* fp, _uint iNumBones);
	HRESULT Ready_Animations(FILE* fp, _uint iNumAnimations);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath);
	static CModel* Create_FromData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType,
		const vector<CBone*> bones, vector<CMesh*>& meshes, vector<CMaterial*>& materials, vector<CAnimation*>& animations);
	virtual CComponent* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END