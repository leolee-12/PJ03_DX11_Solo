//Model.cpp
//#include "..\Public\Model.h"
//#include "Mesh.h"
//#include "Texture.h"
//#include "Bone.h"
//#include "Animation.h"
//
//CModel::CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
//	: CComponent(pDevice, pContext)
//{
//}
//
//CModel::CModel(const CModel& rhs)
//	: CComponent(rhs)
//	, m_PivotMatrix(rhs.m_PivotMatrix)
//	, m_eModelType(rhs.m_eModelType)
//	, m_iNumMeshes(rhs.m_iNumMeshes)
//	, m_Meshes(rhs.m_Meshes)
//	, m_iNumMaterials(rhs.m_iNumMaterials)
//	, m_Materials(rhs.m_Materials)
//	, m_iNumAnimations(rhs.m_iNumAnimations)
//	, m_iRootBoneIndex(rhs.m_iRootBoneIndex)
//	,m_AnimationsName(rhs.m_AnimationsName)
//{
//	for (auto& pPrototypeAnimation : rhs.m_Animations)
//		m_Animations.push_back(pPrototypeAnimation->Clone());
//
//	for (auto& pPrototypeBone : rhs.m_Bones)
//		m_Bones.push_back(pPrototypeBone->Clone());
//
//	for (auto& MaterialDesc : m_Materials)
//	{
//		for (auto& pTexture : MaterialDesc.pMtrlTextures)
//			Safe_AddRef(pTexture);
//	}
//
//	for (auto& pMesh : m_Meshes)
//	{
//		Safe_AddRef(pMesh);
//	}
//
//	strcpy_s(m_szRootBoneName, rhs.m_szRootBoneName);
//}
//
//void CModel::Play_Animation(_cref_time fTimeDelta)
//{
//	if (m_bIsPlay && !m_Animations.empty())
//	{
//		CAnimation* pPrevAnimation = { nullptr };
//		if (m_iCurrentAnimIndex != m_iPrevAnimIndex)
//		{
//			pPrevAnimation = m_Animations[m_iPrevAnimIndex];
//		}
//		m_iPrevAnimIndex = m_iCurrentAnimIndex;
//
//		if (m_iCurrentAnimIndex >= m_iNumAnimations)
//			return;
//
//		/* 현재 애니메이션이 사용하고 있는 뼈들의 TransformationMatrix를 갱신한다. */
//		m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_bIsLoop, fTimeDelta, m_Bones, pPrevAnimation);
//
//		/*RootAnimation 보정*/
//		CBone* pRootBone = m_Bones[m_iRootBoneIndex];
//		_float4x4 RootMatrix = pRootBone->Get_TransformationMatrix();
//
//		if (!strcmp(m_Animations[m_iCurrentAnimIndex]->Get_Name(), "Stand1") || !strcmp(m_Animations[m_iCurrentAnimIndex]->Get_Name(), "Stand2"))
//			m_vRootPosition = { 0.f, 0.f, 0.f };
//		else
//			m_vRootPosition = { RootMatrix._41, 0.f,RootMatrix._43 };
//
//		RootMatrix._41 = 0.f;
//		RootMatrix._43 = 0.f;
//		pRootBone->Set_TransformationMatrix(XMLoadFloat4x4(&RootMatrix));
//
//		/* 화면에 최종적인 상태로 그려내기위해서는 반드시 뼈들의 CombinedTransformationMatrix가 갱신되어야한다. */
//		/* 모든 뼈들을 다 갱신하며 부모로부터 자식까지 쭈우우욱돌아서 CombinedTransformationMatrix를 갱신한다. */
//		for (auto& pBone : m_Bones)
//		{
//			pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
//		}
//	}
//}
//
//void CModel::Set_Animation(_uint iAnimIndex, _bool isLoop, _float fSpeed, _float fTransitionDuration, _bool bSameAnimReset)
//{
//	if (!m_Animations.empty())
//	{
//		if (bSameAnimReset && m_iCurrentAnimIndex == iAnimIndex)
//			m_Animations[m_iCurrentAnimIndex]->Reset_Animation();
//		m_iCurrentAnimIndex = iAnimIndex;
//		m_Animations[m_iCurrentAnimIndex]->Set_TransitionDuration(fTransitionDuration);
//		m_Animations[m_iCurrentAnimIndex]->Set_AnimSpeed(fSpeed);
//		m_bIsLoop = isLoop;
//	}
//}
//
//void CModel::Set_Animation(string strAnimName, _bool isLoop, _float fSpeed, _float fTransitionDuration, _bool bSameAnimReset)
//{
//	if (!m_Animations.empty())
//	{
//		if (bSameAnimReset && m_iCurrentAnimIndex == m_AnimationsName[strAnimName] || m_iCurrentAnimIndex != m_AnimationsName[strAnimName])
//			m_Animations[m_AnimationsName[strAnimName]]->Reset_Animation();
//		m_iCurrentAnimIndex = m_AnimationsName[strAnimName];
//		m_Animations[m_iCurrentAnimIndex]->Set_TransitionDuration(fTransitionDuration);
//		m_Animations[m_iCurrentAnimIndex]->Set_AnimSpeed(fSpeed);
//		m_bIsLoop = isLoop;
//	}
//
//}
//
//void CModel::Set_AnimAdjustSpeed(string strAnimName, _float fValue)
//{
//	_uint iAnimIndex = m_AnimationsName[strAnimName];
//	m_Animations[iAnimIndex]->Set_AnimAdjustSpeed(fValue);
//}
//
//void CModel::Set_CurAnimAdjustSpeed(_float fValue)
//{
//	m_Animations[m_iCurrentAnimIndex]->Set_AnimAdjustSpeed(fValue);
//}
//
//_bool CModel::Is_AnimFinished()
//{
//	return m_Animations[m_iCurrentAnimIndex]->Is_Finished();
//}
//
//_bool CModel::Is_AnimArrived(_uint iTrackFrameIndex)
//{
//	return m_Animations[m_iCurrentAnimIndex]->Is_Arrived(iTrackFrameIndex);
//}
//
//_bool CModel::Is_AnimInRage(_uint iMinTrackFrameIndex, _uint iMaxTrackFrameIndex)
//{
//	return m_Animations[m_iCurrentAnimIndex]->Is_Arrived(iMinTrackFrameIndex) && !m_Animations[m_iCurrentAnimIndex]->Is_Arrived(iMaxTrackFrameIndex);
//}
//
//
//
//HRESULT CModel::Initialize_Prototype(MODELTYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix, const _char* strRootBoneName)
//{
//	strcpy_s(m_szRootBoneName, strRootBoneName);
//
//#ifdef _DEBUG
//	if (!Load_BinaryData(strModelFilePath))
//	{		
//		MODEL_DATA ModelData = {};
//		ModelData.eModelType = (MODELTYPE)eType;
//
//		_fmatrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
//
//		_uint	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
//
//		if (MODELTYPE::NONANIM == ModelData.eModelType)
//			iFlag |= aiProcess_PreTransformVertices;
//
//		m_pAIScene = m_Importer.ReadFile(strModelFilePath, iFlag);
//		if (nullptr == m_pAIScene)
//			RETURN_EFAIL;
//
//		if (FAILED(ModelData.Bake_Bones(m_pAIScene->mRootNode, -1)))
//			RETURN_EFAIL;
//
//		if (FAILED(ModelData.Bake_Meshes(m_pAIScene, PivotMatrix)))
//			RETURN_EFAIL;
//
//		if (FAILED(ModelData.Bake_Materials(m_pAIScene, strModelFilePath)))
//			RETURN_EFAIL;
//
//		if (FAILED(ModelData.Bake_Animations(m_pAIScene)))
//			RETURN_EFAIL;
//
//		_char		szDrive[MAX_PATH] = "";
//		_char		szDirectory[MAX_PATH] = "";
//		_char		szFileName[MAX_PATH] = "";
//		_char		szEXT[MAX_PATH] = ".bin";
//
//		_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
//
//		_char		szFullPath[MAX_PATH] = "";
//		strcpy_s(szFullPath, szDrive);
//		strcat_s(szFullPath, szDirectory);
//		strcat_s(szFullPath, szFileName);
//		strcat_s(szFullPath, szEXT);
//
//		string strPath = string(szFullPath);
//
//		ofstream fout;
//
//		fout.open(strPath, std::ofstream::binary);
//
//		if (fout.is_open())
//		{
//			ModelData.Bake_Binary(fout);
//		}
//		else
//			RETURN_EFAIL;
//
//		fout.close();
//		Load_BinaryData(strModelFilePath);
//	}
//#endif
//	
//	{
//		m_eModelType = m_ModelData.eModelType;
//		m_PivotMatrix = m_ModelData.PivotMatrix;
//
//		if (FAILED(Ready_Bones(m_ModelData)))
//			RETURN_EFAIL;
//
//		if (FAILED(Ready_Meshes(m_ModelData)))
//			RETURN_EFAIL;
//
//		if (FAILED(Ready_Materials(m_ModelData)))
//			RETURN_EFAIL;
//
//		if (FAILED(Ready_Animations(m_ModelData)))
//			RETURN_EFAIL;
//	}
//	
//	_uint iNumBones = (_uint) m_Bones.size();
//	for (_uint i = 0; i < iNumBones; i++)
//	{
//		if (!strcmp(m_Bones[i]->Get_Name(),m_szRootBoneName))
//		{
//			m_iRootBoneIndex = i;
//			m_Bones[i]->Set_IsRoot(true);
//		}
//	}
//	
//	return S_OK;
//}
//
//HRESULT CModel::Initialize(void* pArg)
//{
//	return S_OK;
//}
//
//HRESULT CModel::Render(_uint iMeshIndex)
//{
//	if (iMeshIndex >= m_iNumMeshes)
//		RETURN_EFAIL;
//
//	m_Meshes[iMeshIndex]->Bind_VIBuffers();
//	m_Meshes[iMeshIndex]->Render();
//
//	return S_OK;
//}
//
//HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
//{
//	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
//}
//
//HRESULT CModel::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, TEXTURETYPE eTextureType)
//{
//	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
//	if (iMaterialIndex >= m_iNumMaterials)
//		RETURN_EFAIL;
//
//	if(m_Materials[iMaterialIndex].pMtrlTextures[eTextureType] != nullptr)
//		return m_Materials[iMaterialIndex].pMtrlTextures[eTextureType]->Bind_ShaderResource(pShader, pConstantName);
//
//	return S_OK;
//}
//
//_bool CModel::Load_BinaryData(string strPath)
//{
//	_char		szDrive[MAX_PATH] = "";
//	_char		szDirectory[MAX_PATH] = "";
//	_char		szFileName[MAX_PATH] = "";
//	_char		szEXT[MAX_PATH] = ".bin";
//
//	_splitpath_s(strPath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
//
//	_char		szFullPath[MAX_PATH] = "";
//	strcpy_s(szFullPath, szDrive);
//	strcat_s(szFullPath, szDirectory);
//	strcat_s(szFullPath, szFileName);
//	strcat_s(szFullPath, szEXT);
//
//	string strBinPath = string(szFullPath);
//
//	ifstream fin;
//
//	fin.open(strBinPath, std::ios::binary);
//
//	if (fin.is_open())
//	{
//		m_ModelData.Load_Binary(fin);
//	}
//	else
//		return false;
//
//	fin.close();
//	return true;
//}
//
//#ifdef _DEBUG
//HRESULT CModel::Ready_Meshes(_fmatrix PivotMatrix)
//{
//	m_iNumMeshes = m_pAIScene->mNumMeshes;
//
//	m_Meshes.reserve(m_iNumMeshes);
//
//	for (size_t i = 0; i < m_iNumMeshes; i++)
//	{
//		CMesh* pMesh = CMesh::Create(m_eModelType, m_pDevice, m_pContext, m_pAIScene->mMeshes[i], PivotMatrix, m_Bones);
//
//		if (nullptr == pMesh)
//			RETURN_EFAIL;
//
//		m_Meshes.push_back(pMesh);
//	}
//
//	return S_OK;
//}
//#endif
//
//HRESULT CModel::Ready_Meshes(MODEL_DATA& ModelData)
//{
//	m_iNumMeshes = ModelData.iNumMeshes;
//
//	m_Meshes.reserve(m_iNumMeshes);
//
//	for (size_t i = 0; i < m_iNumMeshes; i++)
//	{
//		CMesh* pMesh = CMesh::Create(ModelData.Meshes[i], m_pDevice, m_pContext);
//
//		if (nullptr == pMesh)
//			RETURN_EFAIL;
//
//		m_Meshes.push_back(pMesh);
//	}
//	return S_OK;
//}
//
//#ifdef _DEBUG
//HRESULT CModel::Ready_Materials(const string& strModelFilePath)
//{
//	m_iNumMaterials = m_pAIScene->mNumMaterials;
//
//	for (_uint i = 0; i < m_iNumMaterials; i++)
//	{
//		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];
//		
//		MATERIAL_DESC	MaterialDesc = {};
//
//		for (int j = 1; j < (int)AI_TEXTURE_TYPE_MAX; j++)
//		{
//			_char		szDrive[MAX_PATH] = "";
//			_char		szDirectory[MAX_PATH] = "";
//
//			_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
//
//			aiString			strPath;
//			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
//				continue;
//
//			_char		szFileName[MAX_PATH] = "";
//			_char		szEXT[MAX_PATH] = "";
//
//			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);
//
//			dds사용
//			strcpy_s(szEXT,".dds");
//
//			_char		szTmp[MAX_PATH] = "";
//			strcpy_s(szTmp, szDrive);
//			strcat_s(szTmp, szDirectory);
//			strcat_s(szTmp, szFileName);
//			strcat_s(szTmp, szEXT);
//
//			_tchar		szFullPath[MAX_PATH] = TEXT("");
//
//			MultiByteToWideChar(CP_ACP, 0, szTmp, strlen(szTmp), szFullPath, MAX_PATH);
//
//
//			MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath, 1);
//			if (nullptr == MaterialDesc.pMtrlTextures[j])
//				RETURN_EFAIL;
//		}
//
//		m_Materials.push_back(MaterialDesc);
//	}
//
//	return S_OK;
//}
//#endif
//
//HRESULT CModel::Ready_Materials(MODEL_DATA& ModelData)
//{
//	m_iNumMaterials = ModelData.iNumMaterials;
//
//	for (_uint i = 0; i < m_iNumMaterials; i++)
//	{
//		MATERIAL_DESC	MaterialDesc = {};
//		MATERIAL_DATA	MaterialData = ModelData.Materials[i];
//
//		for (int j = 1; j < (int)18; j++)
//		{
//			wstring wstrPath = StrToWstr(MaterialData.szTextureFullPath[j]);
//			if (wstrPath == L"")
//				continue;
//			MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice, m_pContext, wstrPath, 1);
//			if (nullptr == MaterialDesc.pMtrlTextures[j])
//				RETURN_EFAIL;
//		}
//
//		m_Materials.push_back(MaterialDesc);
//	}
//
//	return S_OK;
//}
//
//#ifdef _DEBUG
//HRESULT CModel::Ready_Bones(aiNode* pAINode, _int iParentIndex)
//{
//	CBone* pBone = CBone::Create(pAINode, iParentIndex);
//	if (nullptr == pBone)
//		RETURN_EFAIL;
//
//	m_Bones.push_back(pBone);
//
//	_int		iParentIdx = (_int)m_Bones.size() - 1;
//
//	for (size_t i = 0; i < pAINode->mNumChildren; i++)
//	{
//		Ready_Bones(pAINode->mChildren[i], iParentIdx);
//	}
//
//	return S_OK;
//}
//#endif
//
//HRESULT CModel::Ready_Bones(MODEL_DATA& ModelData)
//{
//	for (_uint i = 0; i < ModelData.iNumBones; i++)
//	{
//		CBone* pBone = CBone::Create(ModelData.Bones[i]);
//
//		if (nullptr == pBone)
//			RETURN_EFAIL;
//		m_Bones.push_back(pBone);
//	}
//	return S_OK;
//}
//
//#ifdef _DEBUG
//HRESULT CModel::Ready_Animations()
//{
//	m_iNumAnimations = m_pAIScene->mNumAnimations;
//
//	for (size_t i = 0; i < m_iNumAnimations; i++)
//	{
//		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i],m_Bones);
//		if (nullptr == pAnimation)
//			RETURN_EFAIL;
//
//		m_Animations.push_back(pAnimation);
//		m_AnimationsName.emplace(make_pair( string(pAnimation->Get_Name()), i ));
//	} 
//
//	return S_OK;
//}
//#endif
//
//HRESULT CModel::Ready_Animations(MODEL_DATA& ModelData)
//{
//	m_iNumAnimations = ModelData.iNumAnimations;
//
//	for (size_t i = 0; i < m_iNumAnimations; i++)
//	{
//		CAnimation* pAnimation = CAnimation::Create(ModelData.Animations[i]);
//		if (nullptr == pAnimation)
//			RETURN_EFAIL;
//
//		m_Animations.push_back(pAnimation);
//		m_AnimationsName.emplace(make_pair( string(pAnimation->Get_Name()), i ));
//	}
//
//	return S_OK;
//}
//
//#ifdef _DEBUG
//CBone* CModel::Get_BonePtr(const _char* pBoneName) const
//{
//	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)
//		{
//			if (!strcmp(pBone->Get_Name(), pBoneName))
//				return true;
//			return false;
//		});
//
//	if (iter == m_Bones.end())
//		return nullptr;
//
//	return *iter;
//}
//#endif
//
//_bool CModel::Intersect_MousePos(SPACETYPE eSpacetype, _float3* pOut, _matrix matWorld, _float* pLengthOut)
//{
//	_float fMinLength = INFINITY;
//	_float3 vOut = {};
//	_bool bCheck = false;
//	for (auto iter : m_Meshes)
//	{
//		if (iter->Intersect_MousePos(eSpacetype, pOut, matWorld, pLengthOut))
//		{
//			if(*pLengthOut < fMinLength)
//			{
//				fMinLength = *pLengthOut;
//				vOut = *pOut;
//				bCheck = true;
//			}
//		}
//	}
//	*pLengthOut = fMinLength;
//	*pOut = vOut;
//
//	return bCheck;
//}
//
//CModel* CModel::Create(MODELTYPE eType, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const string& strModelFilePath, _fmatrix PivotMatrix, const char* strRootBoneName )
//{
//	CModel* pInstance = new CModel(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype(eType,strModelFilePath, PivotMatrix, strRootBoneName)))
//	{
//		MSG_BOX("Failed to Created : CModel");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//CComponent* CModel::Clone(void* pArg)
//{
//	CModel* pInstance = new CModel(*this);
//
//	/* 원형객체를 초기화한다.  */
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		MSG_BOX("Failed to Cloned : CComponent");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//
//void CModel::Free()
//{
//	__super::Free();
//
//	for (auto& pBone : m_Bones)
//		Safe_Release(pBone);
//
//	m_Bones.clear();
//
//	for (auto& MaterialDesc : m_Materials)
//	{
//		for (auto& pTexture : MaterialDesc.pMtrlTextures)
//			Safe_Release(pTexture);
//	}
//	m_Materials.clear();
//
//	for (auto& pMesh : m_Meshes)
//	{
//		Safe_Release(pMesh);
//	}
//	m_Meshes.clear();
//
//	for (auto& pAnimation : m_Animations)
//	{
//		Safe_Release(pAnimation);
//	}
//	m_Animations.clear();
//
//#ifdef _DEBUG
//	if (false == m_isCloned)
//		m_Importer.FreeScene();
//#endif
//}