#include "Texture.h"
#include "Shader.h"

IMPLEMENT_CLONE(CTexture, CComponent)
IMPLEMENT_CREATE_EX2(CTexture, const wstring&, strTextureFilePath, _uint ,iNumTextures)

CTexture::CTexture(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice,pContext)
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	,m_iNumTextures(rhs.m_iNumTextures)
	,m_SRVs(rhs.m_SRVs)
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	for (size_t i = 0; i < m_iNumTextures; i++)
	{
		/* ..\Á¤ÀÇÈÆ\139\Framework\Client\Bin\Resources\Textures\Default.jpg */
		_tchar		szFullPath[MAX_PATH] = TEXT("");
		/* %d ¿¡ i°ª °áÇÕ */
		wsprintf(szFullPath, strTextureFilePath.c_str(), i);


		///* D:\ */
		//_tchar		szDrive[MAX_PATH] = TEXT("");

		///* Á¤ÀÇÈÆ\139\Framework\Client\Bin\Resources\Textures\ */
		//_tchar		szDirectory[MAX_PATH] = TEXT("");

		///* Default_0 */
		//_tchar		szFileName[MAX_PATH] = TEXT("");

		/* .jpg */
		_tchar		szExt[MAX_PATH] = TEXT("");
		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
		
		ID3D11ShaderResourceView* pSRV = { nullptr };

		HRESULT			hr = 0;

		if (!lstrcmp(szExt, TEXT(".dds")))
		{
			hr = CreateDDSTextureFromFile(m_pDevice.Get(), szFullPath, nullptr, &pSRV);
		}
		else if (!lstrcmp(szExt, TEXT(".tga")))
		{
			hr = E_FAIL;
		}
		else
		{
			hr = CreateWICTextureFromFile(m_pDevice.Get(), szFullPath, nullptr, &pSRV);
		}

		if (FAILED(hr))
			RETURN_EFAIL;

		m_SRVs.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRVs(pConstantName, &m_SRVs.front(), m_iNumTextures);
}


void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();
}
