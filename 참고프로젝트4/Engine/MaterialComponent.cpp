#include "MaterialComponent.h"

#include "GameInstance.h"
#include "MaterialContainer.h"
#include "Shader.h"

IMPLEMENT_CLONE(CMaterialComponent, CComponent)
IMPLEMENT_CREATE(CMaterialComponent)

CMaterialComponent::CMaterialComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: BASE(pDevice, pContext)
{
}

CMaterialComponent::CMaterialComponent(const CMaterialComponent& rhs)
    : BASE(rhs)
	, m_arrTextureComps(rhs.m_arrTextureComps)
{
	
}

HRESULT CMaterialComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMaterialComponent::Initialize(void* pArg)
{
    return S_OK;
}

void CMaterialComponent::Free()
{
	__super::Free();
}

HRESULT CMaterialComponent::Ready_Materials_ByModel(const wstring& strModelKey, _uint iMesh_MatIndex)
{
	if (nullptr == m_pGameInstance)
		RETURN_EFAIL;

	auto pMaterialData = m_pGameInstance->Find_MaterialData(strModelKey, iMesh_MatIndex);
	if (nullptr == pMaterialData)
		RETURN_EFAIL;

	for (_uint i = 1; i < g_iNumTextures; i++)
	{
		// 머터리얼의 존재여부가 있을 때 텍스처를 생성한다.
		if (!pMaterialData->strTexture[i].empty())
		{
			m_arrTextureComps[i] = CTextureComponent::Create(m_pDevice, m_pContext);
			if (FAILED(m_arrTextureComps[i]->Ready_Texture(pMaterialData->strTexture[i], 1, true)))
			{
				// 로드 실패하면 죽이기
				m_arrTextureComps[i].reset();
			}
		}
	}

	return S_OK;
}

HRESULT CMaterialComponent::Ready_CustomSingleMaterial(_uint iMatIndex, const wstring& strTexturePath, _uint iNumTextures)
{
	if (nullptr == m_pGameInstance || iMatIndex < 0 || iMatIndex >= g_iNumTextures)
		RETURN_EFAIL;

	// 텍스처 존재유무 및 준비
	shared_ptr<CTextureComponent> pTextureComp = m_arrTextureComps[iMatIndex];
	if (nullptr == pTextureComp)
		pTextureComp = CTextureComponent::Create(m_pDevice, m_pContext);

	// 텍스처를 메모리에 로드
	if (pTextureComp)
	{
		if (FAILED(pTextureComp->Ready_Texture(strTexturePath, iNumTextures, true)))
			return E_FAIL;

		// 멤버 배열에 기록
		m_arrTextureComps[iMatIndex] = pTextureComp;
	}
	else
		RETURN_EFAIL;

	return S_OK;
}

ID3D11ShaderResourceView* CMaterialComponent::Find_SRV(_uint iMatIndex, _uint iTexIndex)
{
	// 안쓰는 0번은 제외
	if (iMatIndex < 1 || iMatIndex >= g_iNumTextures)
		return nullptr;

	// 머터리얼 번호 체크
	if (nullptr == m_arrTextureComps[iMatIndex])
		return nullptr;

	// 머터리얼 텍스처의 특정 번호의 텍스처를 가져온다.
	return m_arrTextureComps[iMatIndex]->Get_ShaderResourseView(iTexIndex);
}

HRESULT CMaterialComponent::Bind_SRVToShader(CShader* pShader, const _char* pConstantName, _uint iMatIndex, _uint iTexIndex, _bool bCustom)
{
	ID3D11ShaderResourceView* pTexSRV = nullptr;

	if (nullptr == pShader)
		RETURN_EFAIL;

	if (!bCustom && nullptr == m_arrTextureComps[iMatIndex])
	{
		pTexSRV = Find_DefaultSRV(iMatIndex);
	}
	else		
		pTexSRV = m_arrTextureComps[iMatIndex]->Get_ShaderResourseView(iTexIndex);
	if (nullptr == pTexSRV)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, pTexSRV);
}

HRESULT CMaterialComponent::Bind_SRVToShaders(CShader* pShader, const _char* pConstantName, _uint iTexNum)
{
	if (nullptr == pShader)
		RETURN_EFAIL;

	if (iTexNum >= m_arrTextureComps.size())
		RETURN_EFAIL;

	ID3D11ShaderResourceView* pTesSRVs[18] = {};

	for (_uint i = 0; i < iTexNum; i++)
	{
		pTesSRVs[i] = m_arrTextureComps[i]->Get_ShaderResourseView(0);
		if (pTesSRVs[i] == nullptr)
			RETURN_EFAIL;
	}

	return pShader->Bind_SRVs(pConstantName, pTesSRVs, iTexNum);

	/*vector<ID3D11ShaderResourceView*> vecTexSRVs;

	for (_uint i = 0; i < iTexNum; i++)
	{
		vecTexSRVs.push
	}
	*/

	return S_OK;
}

ID3D11ShaderResourceView* CMaterialComponent::Find_DefaultSRV(_uint iMatIndex)
{
	switch(iMatIndex)
	{
	case TextureType_DIFFUSE :
	case TextureType_DIFFUSE_ROUGHNESS:
	case TextureType_EMISSION_COLOR:
	case TextureType_BASE_COLOR:
	case TextureType_METALNESS:
		return m_pGameInstance->Get_Renderer()->Get_DefaultSRV_ZERO();

	case TextureType_NORMALS:
	case TextureType_NORMAL_CAMERA:
		return m_pGameInstance->Get_Renderer()->Get_DefaultSRV_NORMAL();

	case TextureType_OPACITY:
	case TextureType_AMBIENT_OCCLUSION:
		return m_pGameInstance->Get_Renderer()->Get_DefaultSRV_ONE();

	default:
		return m_pGameInstance->Get_Renderer()->Get_DefaultSRV_ZERO();

	}

	return m_pGameInstance->Get_Renderer()->Get_DefaultSRV_ZERO();
}
