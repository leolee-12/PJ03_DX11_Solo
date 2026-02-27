#include "stdafx.h"
#include "AnimObject.h"
#include "CommonModelComp.h"
#include "GameInstance.h"

IMPLEMENT_CREATE(CAnimObject)
IMPLEMENT_CLONE(CAnimObject, CGameObject)

CAnimObject::CAnimObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice, pContext)
{

}

CAnimObject::CAnimObject(const CAnimObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAnimObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAnimObject::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CAnimObject::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CAnimObject::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bIsAnimPlaying)
	{
		m_pModelCom->Tick_AnimTime(fTimeDelta);
	}
	else
		m_pModelCom->Tick_AnimTime(0.f);
}

void CAnimObject::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pGameInstance->Key_PressingEx(DIK_C, DIK_MD_LCTRL))
		m_pTransformCom->Move_To_AnimationPos(m_pModelCom, fTimeDelta);

	cout << m_pModelCom->Get_CurrentAnimationName() << endl;
}

void CAnimObject::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND_CHARACTER, shared_from_this())))
		return;
}

HRESULT CAnimObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	if (m_pGameInstance->Key_DownEx(DIK_C, DIK_MD_LCTRL))
	{
		m_pTransformCom->Move_To_AnimationPos(m_pModelCom, 0.1f);
	}

	return S_OK;
}

HRESULT CAnimObject::Ready_Components(void* pArg)
{
	TMODEL_DESC Desc = {};

	if (nullptr == pArg)
		RETURN_EFAIL;
	Desc = (*ReCast<TMODEL_DESC*>(pArg));

	m_strModelTag = Desc.strModelTag;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	//m_pModelCom = CCommonModelComp::Create(m_pDevice, m_pContext);

	if (m_pModelCom)
	{
		if (FAILED(m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, m_strModelTag)))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
			RETURN_EFAIL;
		m_pModelCom->Set_Animation(0, 1.f, true);
		m_pModelCom->Set_PreRotate(
			XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
		//m_pModelCom->Transform().Set_Scaling(0.01f, 0.01f, 0.01f);
		/*m_pModelCom->Transform().Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
		m_pModelCom->Transform().Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));*/
	}

	m_pCSShader = CComputeShaderComp::Create(m_pDevice, m_pContext);
	if (m_pCSShader)
	{
		m_pCSShader->Link_Shader(TEXT("CS_Basic"), "CS", nullptr);
		if (FAILED(Create_ComputeBuffer()))
			return E_FAIL;
		if (FAILED(Dispatch_ComputeShader()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CAnimObject::Bind_ShaderResources()
{
	// 모델 렌더
	if (m_pModelCom)
	{
		_float4x4 TempFloat4x4 = {};
		TempFloat4x4 = m_pTransformCom->Get_WorldFloat4x4();
		_matrix TempMatrix = m_pModelCom->Transform().Get_WorldMatrix();
		XMStoreFloat4x4(&TempFloat4x4, XMLoadFloat4x4(&TempFloat4x4) * TempMatrix);

		// 공통 행렬 바인딩
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &TempFloat4x4)))
			return E_FAIL;
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
			RETURN_EFAIL;
		if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
			RETURN_EFAIL;

		auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
		for (_uint i = 0; i < ActiveMeshes.size(); ++i)
		{
			// 뼈 바인딩
			if (FAILED(m_pModelCom->Bind_BoneToShader(ActiveMeshes[i], "g_BoneMatrices")))
				RETURN_EFAIL;

			// 텍스처 바인딩
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE, "g_DiffuseTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_NORMALS, "g_NormalTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE_ROUGHNESS, "g_MRVTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_METALNESS, "g_MTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_EMISSION_COLOR, "g_EmissiveTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_AMBIENT_OCCLUSION, "g_AOTexture")))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_OPACITY, "g_AlphaTexture")))
				RETURN_EFAIL;

			// 패스, 버퍼 바인딩
			if (FAILED(m_pModelCom->ShaderComp()->Begin(0)))
				RETURN_EFAIL;
			if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
				RETURN_EFAIL;
		}
	}

	return S_OK;
}

HRESULT CAnimObject::Change_ModelTag(wstring ModelTag)
{
	if (FAILED(m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, ModelTag)))
		RETURN_EFAIL;

	return S_OK;
}

void CAnimObject::Write_Json(json& Out_Json)
{
	//Out_Json["ModelTag"] = m_szModelTag;

	__super::Write_Json(Out_Json);
}

void CAnimObject::Load_Json(const json& In_Json)
{
	// 여기서 오류나는 문제가 있어서 잠시 비활성화 해놨어
	//m_szModelTag = In_Json["ModelTag"];

	__super::Load_Json(In_Json);
}

void CAnimObject::Free()
{
	__super::Free();
}

HRESULT CAnimObject::Create_ComputeBuffer()
{
	if (m_pCSShader == nullptr)
		return E_FAIL;

	_float* initData = new _float[128];
	for (_uint i = 0; i < 128; i++)
	{
		initData[i] = 1;
	}

	// 버퍼 생성. 컴퓨트 셰이더 용
	D3D11_BUFFER_DESC Desc = {};
	Desc.ByteWidth = sizeof(_float) * 128;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	Desc.StructureByteStride = sizeof(_float);

	D3D11_SUBRESOURCE_DATA SubResource;
	SubResource.pSysMem = initData;
	SubResource.SysMemPitch = 0;
	SubResource.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&Desc, &SubResource, m_pBuffer1.GetAddressOf())))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, m_pBuffer2.GetAddressOf())))
		return E_FAIL;

	Safe_Delete_Array(initData);

	Desc = {};
	Desc.ByteWidth = sizeof(_float) * 128;
	Desc.Usage = D3D11_USAGE_STAGING;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Desc.StructureByteStride = sizeof(_float);

	if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, m_pBackBuffer.GetAddressOf())))
		return E_FAIL;


	Desc = {};
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.ByteWidth = sizeof(Constants);
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, m_pConstantBuffer.GetAddressOf())))
		return E_FAIL;

	if (FAILED(Create_ComputeUAV()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimObject::Create_ComputeUAV()
{
	// UAV는 UNKNOWN, BUFFER 타입을 씀.
	D3D11_SHADER_RESOURCE_VIEW_DESC Desc = {};
	Desc.Format = DXGI_FORMAT_UNKNOWN;
	Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	Desc.Buffer.ElementWidth = 128;

	// 셰이더 리소스 생성
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pBuffer1.Get(), &Desc, m_pSRV1.GetAddressOf())))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pBuffer2.Get(), &Desc, m_pSRV2.GetAddressOf())))
		return E_FAIL;



	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV = {};
	DescUAV.Format = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	DescUAV.Buffer.NumElements = 128;


	// UAV 생성
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pBuffer1.Get(), &DescUAV, m_pUAV1.GetAddressOf())))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pBuffer2.Get(), &DescUAV, m_pUAV2.GetAddressOf())))
		return E_FAIL;
	return S_OK;
}

HRESULT CAnimObject::Dispatch_ComputeShader()
{
	// 상수 버퍼 바인드
	m_pContext->CSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
	// 셰이더 설정
	m_pContext->CSSetShader(m_pCSShader->CRef_Shader().Get(), nullptr, 0);

	_uint iDataCount = 128;
	_bool flag = false;
	ID3D11ShaderResourceView* pViewNULL = { nullptr };

	// 연산하기, 대충 셰이더 리소스 등록, 계산 데이터 옮기기, 디스패치
	do
	{
		// 리소스 변경
		flag = !flag;

		// 현재 SRV 해제
		m_pContext->CSSetShaderResources(0, 1, &pViewNULL);

		// UAV 설정
		m_pContext->CSSetUnorderedAccessViews(0, 1, flag ? m_pUAV2.GetAddressOf() : m_pUAV1.GetAddressOf(), nullptr);

		// SRV 설정
		m_pContext->CSSetShaderResources(0, 1, flag ? m_pSRV1.GetAddressOf() : m_pSRV2.GetAddressOf());

		D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
		if (FAILED(m_pContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
			return E_FAIL;

		memcpy(MappedResource.pData, m_pConstantBuffer.GetAddressOf(), sizeof(m_pConstantBuffer.Get()));
		m_pContext->Unmap(m_pConstantBuffer.Get(), 0);

		_uint threadGroup = (128 + 127) / 128;

		m_pContext->Dispatch(threadGroup, 1, 1);

		iDataCount = threadGroup;
	} while (iDataCount > 1);


	// 결과를 버퍼로 받아오기
	m_pContext->CopyResource(m_pBackBuffer.Get(), flag ? m_pBuffer1.Get() : m_pBuffer2.Get());

	D3D11_BOX box;
	box.left = 0; box.right = sizeof(_float);
	box.top = 0; box.bottom = 1;
	box.front = 0; box.back = 1;
	m_pContext->CopySubresourceRegion(m_pBackBuffer.Get(), 0, 0, 0, 0, flag ? m_pBuffer2.Get() : m_pBuffer1.Get(), 0, &box);


	// 결과 읽어와 저장하기
	D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
	if (SUCCEEDED(m_pContext->Map(m_pBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource)))
	{
		_float sum = *(_float*)MappedResource.pData;
		m_pContext->Unmap(m_pBackBuffer.Get(), 0);
	}

	return S_OK;
}