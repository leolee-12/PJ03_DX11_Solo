#include "BoneContainer.h"

#include "GameInstance.h"

FBoneData::FBoneData(const FBoneData& rhs)
	: iID(rhs.iID)
	, strName(rhs.strName), iParentID(rhs.iParentID), iChildrenID(rhs.iChildrenID)
{
}

FBoneData* FBoneData::Create()
{
	ThisClass* pInstance = new ThisClass();

	if (!pInstance)
	{
		MSG_BOX("FBoneNodeData Create Failed");
	}

	return pInstance;
}

FBoneData* FBoneData::Clone()
{
	ThisClass* pInstance = new ThisClass(*this);

	if (!pInstance)
	{
		MSG_BOX("FBoneNodeData Copy Failed");
	}

	return pInstance;
}

void FBoneData::Free()
{

}


// ------------------------ ModelNodeGroup ----------------------------

CBoneGroup::CBoneGroup()
{
}

CBoneGroup::CBoneGroup(const CBoneGroup& rhs)
	: m_BonesVector(rhs.m_BonesVector)
	, m_iNumBones(rhs.m_iNumBones)
	, m_iRootTransBoneInd(rhs.m_iRootTransBoneInd)
	, m_iRootRotateBoneInd(rhs.m_iRootRotateBoneInd)
	, m_iRootScaleBoneInd(rhs.m_iRootScaleBoneInd)
	, m_TransformMatrices(rhs.m_TransformMatrices)
//	, m_OffsetTransformMatrices(rhs.m_OffsetTransformMatrices)
	, m_CombinedTransformMatrices(rhs.m_CombinedTransformMatrices)
//	, m_bUse_VTF(rhs.m_bUse_VTF), m_iNumVTFRows(rhs.m_iNumVTFRows), m_iNumVTFColums(rhs.m_iNumVTFColums)
//	, m_iNumVTF_LastRowColums(rhs.m_iNumVTF_LastRowColums), m_pVertexTexture(rhs.m_pVertexTexture)
//	, m_pVT_SRV(rhs.m_pVT_SRV)
//	, m_TransformBuffer(rhs.m_TransformBuffer), m_TransformSRV(rhs.m_TransformSRV), m_TransformUAV(rhs.m_TransformUAV)
//	, m_CombinedTransformBuffer(rhs.m_CombinedTransformBuffer), m_CombinedTransformSRV(rhs.m_CombinedTransformSRV)
//	, m_CombinedTransformUAV(rhs.m_CombinedTransformUAV)
{
	// 깊은 복사
	for (size_t i = 0; i < m_BonesVector.size(); i++)
	{
		m_BonesVector[i] = m_BonesVector[i]->Clone();
		m_BonesMap.emplace(m_BonesVector[i]->strName, m_BonesVector[i]);
	}

}

CBoneGroup* CBoneGroup::Create()
{
	ThisClass* pInstance = new ThisClass();

	if (!pInstance)
	{
		MSG_BOX("FBoneNodeData Create Failed");
	}

	return pInstance;
}

CBoneGroup* CBoneGroup::Clone()
{
	ThisClass* pInstance = new ThisClass(*this);

	if (!pInstance)
	{
		MSG_BOX("FBoneNodeData Copy Failed");
	}

	return pInstance;
}

void CBoneGroup::Free()
{
	for (auto& Pair : m_BonesMap)
		Safe_Release(Pair.second);
	m_BonesMap.clear();
	m_BonesVector.clear();
}


FBoneData* CBoneGroup::Find_BoneData(_int iID)
{
	if (iID < 0 || iID >= m_BonesVector.size())
		return nullptr;

	return m_BonesVector[iID];
}

FBoneData* CBoneGroup::Find_BoneData(const wstring& strBoneNodeKey)
{
	auto iter = m_BonesMap.find(strBoneNodeKey);
	if (iter == m_BonesMap.end())
		return nullptr;

	return (*iter).second;
}

FBoneData* CBoneGroup::Find_RootTransBoneData()
{
	return Find_BoneData(m_iRootTransBoneInd);
}

FBoneData* CBoneGroup::Find_RootRotateBoneData()
{
	return Find_BoneData(m_iRootRotateBoneInd);
}

FBoneData* CBoneGroup::Find_RootScaleBoneData()
{
	return Find_BoneData(m_iRootScaleBoneInd);
}

HRESULT CBoneGroup::Add_BoneData(const wstring& strBoneNodeKey, FBoneData* pNode, _fmatrix& TransformMatrix)
{
	// ID 추가
	if (pNode->iID >= m_BonesVector.size())
	{
		m_BonesVector.resize(pNode->iID + 1, nullptr);
		m_TransformMatrices.resize(pNode->iID + 1, TransformMatrix);
		//m_OffsetTransformMatrices.resize(pNode->iID + 1, XMMatrixInverse(nullptr, TransformMatrix));
		m_CombinedTransformMatrices.resize(pNode->iID + 1, XMMatrixIdentity());
	}

	// ID 중복은 치명적인 오류임
	if (m_BonesVector[pNode->iID] != nullptr)
		RETURN_EFAIL;

	m_BonesVector[pNode->iID] = pNode;

	// 순서대로 넣는다면 오류가 나지 않을 것.
	if (pNode->iParentID != -1)
	{
		m_BonesVector[pNode->iParentID]->iChildrenID.emplace_back(pNode->iID);
		m_BonesVector[pNode->iParentID]->iChildrenID.shrink_to_fit();
	}

	// 맵에 추가
	auto iter = m_BonesMap.find(strBoneNodeKey);
	if (iter != m_BonesMap.end())
		RETURN_EFAIL;

	m_BonesMap.emplace(strBoneNodeKey, pNode);
	m_iNumBones = Cast<_uint>(m_BonesVector.size());

	return S_OK;
}

HRESULT CBoneGroup::Invalidate_BoneTransform(_uint iIndex, _fmatrix& matTransform)
{
	if (iIndex < 0 || iIndex >= m_BonesVector.size())
		RETURN_EFAIL;

	XMStoreFloat4x4(&m_TransformMatrices[iIndex], matTransform);

	return S_OK;
}

HRESULT CBoneGroup::Invalidate_BoneTransforms(vector<_float4x4>& Transforms, vector<_bool>& Applies)
{
	// 크기가 다르면 치명적인 오류
	if (Transforms.size() != m_iNumBones)
	{
		assert(false);
		return E_FAIL;
	}

	// 변경점이 없는 뼈 처리
	vector<pair<_float4x4, _uint>> BackupMatrix;
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		if (!Applies[i])
			BackupMatrix.push_back({ m_TransformMatrices[i], i });
	}

	// 빠른 복사
	memcpy(&m_TransformMatrices[0], Transforms.data(), sizeof(_float4x4) * m_iNumBones);


	// 변경점이 없는 뼈, 저장값 복사
	for (_uint i = 0; i < Cast<_uint>(BackupMatrix.size()); i++)
	{
		m_TransformMatrices[BackupMatrix[i].second] = move(BackupMatrix[i].first);
	}

	return S_OK;
}

void CBoneGroup::Invalidate_FinalTransforms()
{
	// 계층구조의 행렬을 업데이트 한다.
	for (_uint i = 0; i < m_BonesVector.size(); i++)
	{
		FBoneData* pBone = m_BonesVector[i];

		_matrix matTransform = XMLoadFloat4x4(&m_TransformMatrices[i]);
		_matrix matParentFinalTransform = XMMatrixIdentity();
		
		if (pBone->iParentID != -1)
			matParentFinalTransform = XMLoadFloat4x4(&m_CombinedTransformMatrices[pBone->iParentID]);

		XMStoreFloat4x4(&m_CombinedTransformMatrices[i], (matTransform * matParentFinalTransform));
	}

	// 함수 안에서 VT 사용 유무 체크 후 텍스처에 쓴다.
	//Bind_TransformsToVTF();
}

void CBoneGroup::Provide_ChildrenList(
	vector<_short>& vecBones, const wstring& strBoneNodeKey, const vector<wstring>& strEndBones, _uint iRecursive) const
{
	auto iter = m_BonesMap.find(strBoneNodeKey);
	if (iter == m_BonesMap.end())
	{
		return;
	}

	vector<_short> vecEndBones;
	for (_uint i = 0; i < (_uint)strEndBones.size(); i++)
	{
		auto iterEndBone = m_BonesMap.find(strEndBones[i]);
		if (iterEndBone == m_BonesMap.end())
		{
			continue;
			assert(false);
		}

		vecEndBones.emplace_back((*iterEndBone).second->iID);
	}
	sort(vecEndBones.begin(), vecEndBones.end());

	// 필터링에 걸리면 돌아감 
	auto iterFind = find_if(vecEndBones.begin(), vecEndBones.end(), [this, &iter](const _short& iID) {
		return iID == m_BonesVector[(*iter).second->iID]->iID || iID == m_BonesVector[(*iter).second->iID]->iParentID;
		});
	if (iterFind != vecEndBones.end())
		return;

	// 반복이 0이면 종료
	if (iRecursive-- <= 0U)
		return;
	// 본인 뼈 포함
	vecBones.emplace_back((*iter).second->iID);
	
	for (_uint i = 0; i < (_uint)(*iter).second->iChildrenID.size(); i++)
	{
		_short iChildID = (*iter).second->iChildrenID[i];
		Provide_ChildrenList_Recursive(vecBones, vecEndBones, iChildID, iRecursive);
	}
	sort(vecBones.begin(), vecBones.end());
}

void CBoneGroup::Provide_ChildrenList_Recursive(vector<_short>& vecBones, const vector<_short>& vecEndBones, const _short& iIndex, _uint iRecursive) const
{
	// 재귀 제한
	if (iRecursive-- <= 0)
		return;
	// 필터링에 걸리면 돌아감 
	auto iter = find_if(vecEndBones.begin(), vecEndBones.end(), [this, &iIndex](const _short& iID) {
		return iID == m_BonesVector[iIndex]->iID || iID == m_BonesVector[iIndex]->iParentID;
		});
	if (iter != vecEndBones.end())
		return;

	vecBones.emplace_back(m_BonesVector[iIndex]->iID);

	for (_uint i = 0; i < (_uint)m_BonesVector[iIndex]->iChildrenID.size(); i++)
	{
		_short iChildID = m_BonesVector[iIndex]->iChildrenID[i];
		Provide_ChildrenList_Recursive(vecBones, vecEndBones, iChildID, iRecursive);
	}
}

void CBoneGroup::Reserve_BoneSize(_uint iNumBoneSize)
{
	m_BonesVector.reserve(iNumBoneSize);
	m_TransformMatrices.reserve(iNumBoneSize);
	m_CombinedTransformMatrices.reserve(iNumBoneSize);
}

void CBoneGroup::Set_PreRotate(_matrix Matrix)
{
	if (m_iNumBones == 0)
		return;

	m_TransformMatrices[0] *= Matrix;
	//m_OffsetTransformMatrices[0] *= XMMatrixInverse(nullptr, Matrix);
	Invalidate_FinalTransforms();
}

/* Deprecated
HRESULT CBoneGroup::Create_VTF_Bones()
{
	if (m_iNumBones < 256)
		return E_FAIL;

	m_bUse_VTF = true;

	_int iFactor = sqrt(m_iNumBones / 1024.f);		// 지수값 계산
	_int iBoneGroups = pow(2, iFactor);				// 최대 텍스처 줄 수 계산
	m_iNumVTFColums = 4096 / 4;						// 줄당 요소 개수(행렬 기준)
	m_iNumVTF_LastRowColums = m_iNumBones % 1024;	// 마지막 행에 들어가는 공간

	D3D11_TEXTURE2D_DESC Desc = {};
	Desc.Width = 4096;
	Desc.Height = m_iNumVTFRows = iBoneGroups;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	
	Desc.SampleDesc.Quality = 0;
	Desc.SampleDesc.Count = 1;

	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;

	if (FAILED(GI()->Get_D3D11Device()->CreateTexture2D(&Desc, nullptr, m_pVertexTexture.GetAddressOf())))
		return E_FAIL;

	// 셰이더 리소스
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = Desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(GI()->Get_D3D11Device()->CreateShaderResourceView(m_pVertexTexture.Get(), &SRVDesc, m_pVT_SRV.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoneGroup::Bind_TransformsToVTF()
{
	if (!m_bUse_VTF || m_pVertexTexture == nullptr)
		return E_FAIL;

	auto pContext = GI()->Get_D3D11Context();

	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(pContext->Map(m_pVertexTexture.Get(), 0, D3D11_MAP_WRITE, D3D11_MAP_WRITE_DISCARD, &mapped)))
	{
		for (_uint i = 0; i < m_iNumVTFRows; i++)
		{
			if (i < m_iNumVTFRows - 1)
				memcpy(&mapped.pData, m_CombinedTransformMatrices.data(), sizeof(_float4x4) * m_iNumVTFColums);
			else
				memcpy(&mapped.pData, m_CombinedTransformMatrices.data(), sizeof(_float4x4) * m_iNumVTF_LastRowColums);
		}

		pContext->Unmap(m_pVertexTexture.Get(), 0);
	}

	return S_OK;
}

HRESULT CBoneGroup::Create_TransformBuffers()
{
	// 뼈의 행렬을 바인딩한다.
	{
		D3D11_BUFFER_DESC Desc = {};
		Desc.ByteWidth = sizeof(_float4x4) * m_iNumBones;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		Desc.CPUAccessFlags = 0;
		Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		Desc.StructureByteStride = sizeof(_float4x4);


		_float4x4* TransformInit = new _float4x4[m_iNumBones];
		memcpy(TransformInit, m_TransformMatrices.data(), sizeof(_float4x4) * m_iNumBones);


		D3D11_SUBRESOURCE_DATA Data = {};
		Data.pSysMem = TransformInit;

		if (FAILED(GI()->Get_D3D11Device()->CreateBuffer(&Desc, &Data, m_TransformBuffer.GetAddressOf())))
		{
			Safe_Delete_Array(TransformInit);
			return E_FAIL;
		}
		Safe_Delete_Array(TransformInit);


		// SRV 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV = {};
		DescSRV.Format = DXGI_FORMAT_UNKNOWN;
		DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescSRV.Buffer.ElementWidth = m_iNumBones;

		if (FAILED(GI()->Get_D3D11Device()->CreateShaderResourceView(m_TransformBuffer.Get(), &DescSRV,
			m_TransformSRV.GetAddressOf())))
			return E_FAIL;


		// UAV 생성
		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV = {};
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.NumElements = m_iNumBones;

		if (FAILED(GI()->Get_D3D11Device()->CreateUnorderedAccessView(m_TransformBuffer.Get(), &DescUAV,
			m_TransformUAV.GetAddressOf())))
			return E_FAIL;
	}



	{
		D3D11_BUFFER_DESC Desc = {};
		Desc.ByteWidth = sizeof(_float4x4) * m_iNumBones;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		Desc.CPUAccessFlags = 0;
		Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		Desc.StructureByteStride = sizeof(_float4x4);


		if (FAILED(GI()->Get_D3D11Device()->CreateBuffer(&Desc, nullptr, m_CombinedTransformBuffer.GetAddressOf())))
			return E_FAIL;


		// SRV 생성
		D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV = {};
		DescSRV.Format = DXGI_FORMAT_UNKNOWN;
		DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescSRV.Buffer.ElementWidth = m_iNumBones;

		if (FAILED(GI()->Get_D3D11Device()->CreateShaderResourceView(m_CombinedTransformBuffer.Get(), &DescSRV,
			m_CombinedTransformSRV.GetAddressOf())))
			return E_FAIL;


		// UAV 생성
		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV = {};
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.NumElements = m_iNumBones;

		if (FAILED(GI()->Get_D3D11Device()->CreateUnorderedAccessView(m_CombinedTransformBuffer.Get(), &DescUAV,
			m_CombinedTransformUAV.GetAddressOf())))
			return E_FAIL;
	}

	return S_OK;
}
*/