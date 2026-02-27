#include "ModelContainer.h"
#include "GameInstance.h"
#include "Shader.h"

HRESULT FModelData::Initialize()
{
	pMeshGroup = CMeshGroup::Create();
	pBoneGroup = CBoneGroup::Create();
	pAnimGroup = CBoneAnimGroup::Create();
	pMaterialGroup = CMaterialGroup::Create();

	return S_OK;
}

FModelData* FModelData::Create(const _bool bLoaded)
{
	ThisClass* pInstance = new ThisClass();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("FModelData Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void FModelData::Free()
{
	Safe_Release(pMeshGroup);
	Safe_Release(pMaterialGroup);
	Safe_Release(pBoneGroup);
	Safe_Release(pAnimGroup);
}

FMeshData* FModelData::Find_Mesh(const _uint iIndex)
{
	if (nullptr == pMeshGroup)
		return nullptr;

	return pMeshGroup->Find_Mesh(iIndex);
}

FMeshData* FModelData::Find_Mesh(const wstring& strMesh, const _uint iRangeIndex)
{
	if (nullptr == pMeshGroup)
		return nullptr;

	return pMeshGroup->Find_Mesh(strMesh, iRangeIndex);
}

FMaterialData* FModelData::Find_Material(const _uint iIndex)
{
	if (nullptr == pMaterialGroup)
		return nullptr;

	return pMaterialGroup->Find_Material(iIndex);
}

FMaterialData* FModelData::Find_Material(const wstring& strMaterial)
{
	if (nullptr == pMaterialGroup)
		return nullptr;

	return pMaterialGroup->Find_Material(strMaterial);
}

CBoneGroup* FModelData::Find_BoneGroup()
{
	if (nullptr == pBoneGroup)
		return nullptr;

	return pBoneGroup;
}


FBoneAnimData* FModelData::Find_BoneAnim(const _uint iIndex)
{
	if (nullptr == pAnimGroup)
		return nullptr;

	return pAnimGroup->Find_BoneAnim(iIndex);
}

FBoneAnimData* FModelData::Find_BoneAnim(const wstring& strBoneAnim)
{
	if (nullptr == pAnimGroup)
		return nullptr;

	return pAnimGroup->Find_BoneAnim(strBoneAnim);
}

HRESULT FModelData::Create_VTF_Texture()
{
	// 텍스쳐
	// 위치 + 노멀 + 탄젠트 + 텍스쿨드 = 44 * 정점 개수(클라우드는 110000개) = 4,840,000 BYTE임
	// 오프셋 행렬 64 * 뼈개수 (560개니까 64 * 560), 행렬 넘겨줄 때 기존은 가진 뼈개수로 넘겼는데
	// 그 방법은 인덱스 찾는데 문제를 일으키기 때문에 사이즈를 다 같게 맞춘다. (실제로 전에도 비포함된
	// 뼈가 있으면 이상하게 출력되기는 했었다. 때문에 전체 뼈 인덱스에 곱해서 넘겨주어야 한다.
	// 애니메이션 적용 행렬, 똑같이 뼈개수 적용 (똑같이 560 * 64)...이건 Validate 값
	// 

	// 순서 -> 메쉬 오프셋 행렬 적용, 인스턴스(정점, )

	// 정점 개수 sizeof(VTX_VTFANIMMODEL)(44) * 정점개수 = 4840000
	// 4840000 / sizeof(FLOAT4) = 필요 픽셀개수(302,500)
	// 4096이면 적어도 73.8, 즉 74줄이 필요함. 하지만 패딩까지 고려해야함.
	// 4096 사이즈를 고정으로 해두자. 정점은 한줄당 20바이트 패딩해야함. 1489개의 정점 들어감.
	// 11000 / 1489
	// 뼈 행렬은 한줄당 1,024개씩 들어간다.
	// 뼈 줄수 = ceil(뼈개수 / 4) = 560 / 1,024 = 올림하면 1줄 * 메쉬개수 = 1 * 12개였나 12줄 개쩐다.
	// 남는 공간은 걍 버림
	// 오프셋 행렬들은 변하면 안된다. 즉, 가장 앞에 두는게 좋겠다.
	// 애니메이션 뼈 행렬, 위와 같음. 단, 인스턴스 개수만큼 길이를 늘인다. 즉, 1줄 * 인스턴스 개수
	// (1줄 + 74줄) * 인스턴스 개수가 된다. 이 부분은 최적화가 되어야 인스턴싱이 의미가 있을 듯.
	// 4096 * 4096 텍스처는 268메가 정도 먹는다.
	// 4,084줄 / 75줄 = 54 인스턴스 개체를 넣는게 가능...근데 이거 그래도 프레임 떨어질걸?
	// 키프레임 계산이 CPU에 있음.

	//struct TVertexTexture
	//{
	//	_float3 vPos;
	//	_float3 vNormal;
	//	_float2 vTexcoord;
	//	_float3 vTangent;
	//};

	//// 초기화 코드 (정점)
	//D3D11_SUBRESOURCE_DATA InitData;
	//InitData.pSysMem = { nullptr };
	//InitData.SysMemPitch = 10;
	//InitData.SysMemSlicePitch = 10;

	//// 메쉬의 
	//size_t iInitDataIndex = 0;
	//_uint iRowStride = 4096 * sizeof(_float4);
	//_uint iNumMeshes = pMeshGroup->Get_NumMeshes();
	//for (_uint i = 0; i < iNumMeshes; i++)
	//{
	//	FMeshData* pMeshData = pMeshGroup->Find_Mesh(i);

	//	if (pMeshData->pVB != nullptr)
	//	{
	//		void* pVertexData = { nullptr };
	//		if (eModelType == TYPE_MODEL)
	//			pVertexData = new VTXMESH[pMeshData->iNumVertices];
	//		else
	//			pVertexData = new VTXMESHANIM[pMeshData->iNumVertices];

	//		if (pVertexData == nullptr)
	//			RETURN_EFAIL;
	//		D3D11_PRIMITIVE_
	//		// 버퍼에서 값을 읽어와서 밑에서 작업하도록 한다.
	//		D3D11_MAPPED_SUBRESOURCE SubResource;
	//		if (SUCCEEDED(GI()->
	//			Get_D3D11Context()->Map(pMeshData->pVB.Get(), 0, D3D11_MAP_READ, 0, &SubResource)))
	//		{
	//			if (eModelType == TYPE_MODEL)
	//				memcpy(pVertexData, SubResource.pData, sizeof(VTXMESH) * pMeshData->iNumVertices);
	//			else
	//				memcpy(pVertexData, SubResource.pData, sizeof(VTXMESHANIM) * pMeshData->iNumVertices);

	//			GI()->Get_D3D11Context()->Unmap(pMeshData->pVB.Get(), 0);
	//		}
	//		
	//		// 정점 패딩값
	//		_uint iPadding = (4096 * (_uint)sizeof(_float4)) % sizeof(TVertexTexture);	// 20
	//		// 텍스처에 집어넣어주기 위해 memcpy, 패딩값 넣어주면서 반복문 돌려야한다.
	//		TVertexTexture* szVertices = new TVertexTexture[pMeshData->iNumVertices];
	//		_uint iCounting = 0;
	//		for (_uint i = 0; i < pMeshData->iNumVertices; i++)
	//		{
	//			for (_uint j = 0; j < pMeshData->iNumV; j++)
	//			{

	//			}
	//			if ((4096 * (_uint)sizeof(_float4)) / (_float)sizeof(TVertexTexture) < )
	//			{
	//				++iCounting;
	//				continue;
	//			}
	//			_uint iIndex = i * sizeof(TVertexTexture) + iCounting * iPadding;
	//			memcpy(&((_char*)szVertices)[i], &((VTXMESHANIM*)pVertexData)[i], sizeof(TVertexTexture));
	//		}
	//	}
	//}
	//

	//D3D11_TEXTURE2D_DESC Desc = {};
	//Desc.Width = 4096;
	//Desc.Height = 128;
	//Desc.MipLevels = 1;
	//Desc.ArraySize = 1;
	//Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//
	//Desc.SampleDesc.Quality = 0;
	//Desc.SampleDesc.Count = 1;

	//Desc.Usage = D3D11_USAGE_DYNAMIC;
	//Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	//Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//Desc.MiscFlags = 0;

	//if (FAILED(GI()->Get_D3D11Device()->CreateTexture2D(&Desc, &InitData, m_pVertexTexture.GetAddressOf())))
	//	RETURN_EFAIL;

	//// 셰이더 리소스
	//D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	//SRVDesc.Format = Desc.Format;
	//SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//SRVDesc.Texture2D.MipLevels = 1;

	//if (FAILED(GI()->Get_D3D11Device()->CreateShaderResourceView(m_pVertexTexture.Get(), &SRVDesc, m_pVT_SRV.GetAddressOf())))
	//	RETURN_EFAIL;

	//// 렌더 타겟
	//D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};
	//RTVDesc.Format = Desc.Format;
	//RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//RTVDesc.Texture2D.MipSlice = 1;

	//if (FAILED(GI()->Get_D3D11Device()->CreateRenderTargetView(m_pVertexTexture.Get(), &RTVDesc, m_pVT_RTV.GetAddressOf())))
	//	RETURN_EFAIL;

	return S_OK;
}

HRESULT FModelData::Bind_ShaderResources(CShader* pShader)
{
	/*if (pShader == nullptr)
		RETURN_EFAIL;

	if (FAILED(pShader->Bind_SRV("g_VertexTexture", m_pVT_SRV.Get())))
		RETURN_EFAIL;*/

	return S_OK;
}

HRESULT FModelData::Bind_VTFBuffers()
{
	//if (m_pVT_SRV == nullptr || m_pVT_RTV == nullptr)
	//	RETURN_EFAIL;

	//auto pContext = GI()->Get_D3D11Context().Get();

	//for(_uint i = 0; i < pMeshGroup->Get_NumMeshes(); ++i)
	//{
	//	auto pMesh = pMeshGroup->Find_Mesh(i);
	//	ID3D11Buffer* pVBs[] = {
	//		pMesh->pVB.Get()
	//	};

	//	_uint		iStrides[] = {
	//		pMesh->iVertexStride
	//	};

	//	_uint		iOffsets[] = {
	//		0,
	//	};

	//	// 버텍스 버퍼 이용
	//	pContext->IASetVertexBuffers(0, 1, pVBs, iStrides, iOffsets);

	//	// 점 토폴로지
	//	pContext->IASetPrimitiveTopology(m_eVTF_Topology);

	//	// 바로 그리기
	//	pContext->Draw(0, 0);
	//}

	return S_OK;
}

HRESULT FModelData::Render_ToVTF(CShader* pShader)
{
	/*if (FAILED(Bind_ShaderResources(pShader)))
		RETURN_EFAIL;

	if (FAILED(pShader->Begin(0)))
		RETURN_EFAIL;

	if (FAILED(Bind_VTFBuffers()))
		RETURN_EFAIL;*/

	return S_OK;
}

