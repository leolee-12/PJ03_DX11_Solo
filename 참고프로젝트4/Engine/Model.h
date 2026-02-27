//Model.h
#pragma once

#include "Component.h"
#include "Model_Data.h"
BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eType, const string & strModelFilePath, _fmatrix PivotMatrix, const _char* strRootBoneName);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_cref_time fTimeDelta);

	void  Set_Animation(_uint iAnimIndex, _bool isLoop, _float fSpeed = 1.f, _float fTransitionDuration = 0.2f, _bool bSameAnimReset = false);
	void  Set_Animation(string strAnimName, _bool isLoop, _float fSpeed = 1.f, _float fTransitionDuration = 0.2f, _bool bSameAnimReset = false);
	void  Set_AnimAdjustSpeed(string strAnimName, _float fValue); 
	void  Set_CurAnimAdjustSpeed(_float fValue);

	void  Set_AnimPlay(_bool value) { m_bIsPlay = value; }
	
	_bool Is_AnimFinished();
	_bool Is_AnimArrived(_uint iTrackFrameIndex);
	_bool Is_AnimInRage(_uint iMinTrackFrameIndex, _uint iMaxTrackFrameIndex);

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, TEXTURETYPE eTextureType);
	
	_bool	Load_BinaryData(string strPath);


#ifdef _DEBUG
private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer;
#endif

	MODEL_DATA				m_ModelData = {};
private:
	/*RootBone*/
	_float3					m_vRootPosition = {};
	_uint					m_iRootBoneIndex = { 0 };
	_char					m_szRootBoneName[MAX_PATH] = "";
	_bool					m_bIsLoop = { true };
	_bool					m_bIsPlay = { true };

	_float4x4				m_PivotMatrix;
	MODELTYPE				m_eModelType = { MODELTYPE::MODELTYPE_END};

	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

	_uint					m_iNumMaterials = { 0 };
	vector<MATERIAL_DESC>	m_Materials;

	_uint							m_iNumAnimations = { 0 };
	_uint							m_iPrevAnimIndex = { 0 };
	_uint							m_iCurrentAnimIndex = { 0 };
	vector<class CAnimation*>		m_Animations;
	map<string, _uint>				m_AnimationsName;

	vector<class CBone*>	m_Bones;
public:
	typedef vector<class CBone*>	BONES;

private:

#ifdef _DEBUG
	HRESULT	Ready_Meshes(_fmatrix PivotMatrix);
	HRESULT Ready_Bones(aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Materials(const string & strModelFilePath); //Material == Texture
	HRESULT Ready_Animations();
#endif

	HRESULT	Ready_Meshes(MODEL_DATA& ModelData);
	HRESULT Ready_Materials(MODEL_DATA& ModelData);
	HRESULT Ready_Bones(MODEL_DATA& ModelData);
	HRESULT Ready_Animations(MODEL_DATA& ModelData);
	
	

public:
	_vector	Get_RootPosition() { return XMLoadFloat3(&m_vRootPosition); }
	CBone*  Get_BonePtr(const _char* pBoneName) const;
	MODELTYPE Get_ModelType() { return m_eModelType; }
	_uint	Get_NumAnimations() { return m_iNumAnimations; }
	_uint	Get_CurrentAnimIndex() { return m_iCurrentAnimIndex; }
	CAnimation* Get_CurrentAnimation() { return m_Animations[m_iCurrentAnimIndex]; }
	map<string, _uint>* Get_AnimationsName() { return &m_AnimationsName; }
	/*For_Tool*/
	_bool	Intersect_MousePos(SPACETYPE eSpacetype, _float3 * pOut, _matrix matWorld, _float * pLengthOut = nullptr);

public:
	static CModel* Create(MODELTYPE eType, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const string & strModelFilePath, _fmatrix PivotMatrix, const _char* strRootBoneName = "Bip001" );
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;
};

END