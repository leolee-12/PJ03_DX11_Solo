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
	_uint Get_MeshMaterialIndex(_uint iMeshIdx);

	_int Get_BoneIndex(const _char* pBoneName);
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;
	const _float3& Get_RootMotionDelta() const { return m_vRootMotionDelta; }
	void Set_AnimationIndex(_uint iIndex, _bool isLoop = false, _float fBlendDuration = 0.f);
	void Set_RootMotionBoneIndex(_uint iIndex) { m_iRootBoneIndex = iIndex; }
	void Set_EnableRootMotion(_bool bEnable) { m_bEnableRootMotion = bEnable; }

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	_bool Play_Animation(_float fTimeDelta);
	void Update_Blend(_float fTimeDelta);
	HRESULT Render(_uint iMeshIndex);
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstName, _uint iMeshIndex, MATERIAL_TYPE eType, _uint iIndex);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstName, _uint iMeshIndex);

private:
	const _string m_pModelFilePath = {};
	MODEL m_eType = { MODEL::END };
	
	_uint m_iNumMeshes = {};
	vector<class CMesh*> m_Meshes;

	_uint m_iNumMaterials = {};
	vector<class CMaterial*> m_Materials;

	_uint m_iNumBones = {};
	vector<class CBone*> m_Bones;
	
	_uint m_iCurrentAnimationIndex = {};
	_uint m_iNumAnimations = {};
	_bool m_isAnimLoop = { false };
	vector<class CAnimation*> m_Animations;

	_bool m_isBlending = { false };
	_float m_fBlendDuration = {};
	_float m_fBlendElapsed = {};
	vector<BONE_SRT> m_BlendSnapshots;
	vector<_bool> m_BlendTargetMask;	// true : 보간 대상
	const unordered_set<_uint>* m_pNextChanneledSet = { nullptr };

	_uint m_iRootBoneIndex = { 0 };			// 바이너리화 로직에서 항상 0번으로 등록
	_bool m_bEnableRootMotion = { false };	// 루트 모션 활성화 플래그
	_float3 m_vRootMotionDelta = {};		// 이번 프레임의 루트 이동 델타(로컬)
	_float3 m_vPrevRootPos = {};			// 이전 프레임 루트 위치(델타 계산용)

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