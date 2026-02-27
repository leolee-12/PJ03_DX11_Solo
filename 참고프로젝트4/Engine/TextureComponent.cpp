#include "TextureComponent.h"

#include "GameInstance.h"
#include "Texture_Manager.h"
#include "Shader.h"

IMPLEMENT_CLONE(CTextureComponent, CComponent)
IMPLEMENT_CREATE(CTextureComponent)

CTextureComponent::CTextureComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: Base(pDevice, pContext)
{
}

CTextureComponent::CTextureComponent(const CTextureComponent& rhs)
    : Base(rhs)
	, m_pTexture_Manager(rhs.m_pTexture_Manager), m_SRVs(rhs.m_SRVs)
{
	Safe_AddRef(m_pTexture_Manager);
}

HRESULT CTextureComponent::Initialize_Prototype()
{
	if (FAILED(Link_TextureManager()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CTextureComponent::Initialize(void* Arg)
{
    return S_OK;
}


void CTextureComponent::Free()
{
	__super::Free();

	Safe_Release(m_pTexture_Manager);
}

ID3D11ShaderResourceView* CTextureComponent::Get_ShaderResourseView(const _uint iIndex)
{
	if (iIndex < 0 || iIndex >= m_iNumTextures)
		return nullptr;

	return m_SRVs[iIndex].Get();
}

HRESULT CTextureComponent::Get_ShaderResourceViews(ID3D11ShaderResourceView** Arr, const _uint iNumTextures)
{
	// 텍스처가 없거나, 사이즈가 맞지 않으면 취소한다.
	if (m_iNumTextures == 0 || m_iNumTextures != iNumTextures)
		return E_FAIL;

	for (_uint i = 0; i < iNumTextures; i++)
		Arr[i] = m_SRVs[i].Get();

	return S_OK;
}

HRESULT CTextureComponent::Ready_Texture(const wstring& strFilePath, const _uint iNumTextures, _bool bUseMainPath)
{
	if (iNumTextures == 0)
	{
		MSG_BOX("TextureComponent : 이상한 값 넣지 마시오~");
		return E_FAIL;
	}

	// 일단은 텍스처 매니저에 종속적으로 만든다.
	if (FAILED(Link_TextureManager()))
		return E_FAIL;

	// 실패시 이미 있다는 거니 찾는다.
	if (FAILED(m_pTexture_Manager->Ready_Texture(strFilePath, iNumTextures, false, bUseMainPath)))
	{
		// 로드 실패하면 그냥 밑으로 내려가서 찾는거다.
	}

	// 찾았는데 없으면 치명적인 오류
	if (FAILED(m_pTexture_Manager->Reference_SRVs(strFilePath, m_SRVs)))
		return E_FAIL;

	// 성공시 텍스처 개수를 업데이트
	m_iNumTextures = Cast<_uint>(m_SRVs.size());

	return S_OK;
}

HRESULT CTextureComponent::Release_Texture()
{
	for (auto& SRV : m_SRVs)
		SRV.Reset();
	m_SRVs.clear();
	m_iNumTextures = 0;

	return S_OK;
}

HRESULT CTextureComponent::Bind_SRVToEffect(CShader* pShader, const _char* pTextureSymbolName, const _uint iIndex)
{
	if (iIndex < 0 || iIndex >= m_iNumTextures)
		RETURN_EFAIL;

	return pShader->Bind_SRV(pTextureSymbolName, m_SRVs[iIndex].Get());
}

HRESULT CTextureComponent::Link_TextureManager()
{
	// 이미 연결됨
	if (nullptr != m_pTexture_Manager)
		return S_OK;

	// 일단은 매니저에 종속적으로 만듬.
	m_pTexture_Manager = m_pGameInstance->Get_TextureManager();
	if (nullptr == m_pTexture_Manager)
		RETURN_EFAIL;

	_uint iCount = Safe_AddRef(m_pTexture_Manager);

#ifdef TEXTURE_COMP_DEBUG
#if TEXTURE_COMP_DEBUG
	cout << "텍스쳐 매니저" << iCount << endl;
#endif
#endif // TEXTURE_COMP_DEBUG

	

	return S_OK;
}
