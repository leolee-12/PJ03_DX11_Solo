#include "Texture.h"

#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{

}

CTexture::CTexture(const CTexture& Prototype)
	: CComponent{ Prototype }
	, m_iNumTextures { Prototype.m_iNumTextures }
	, m_Textures { Prototype.m_Textures}
{
	for (auto& pSRV : m_Textures)
		Safe_AddRef(pSRV);

	

}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	/* ../Bin/Resources/Textures/Explosion/Explosion.png */
	/* ../Bin/Resources/Textures/Explosion/Explosion%d.png */

	m_iNumTextures = iNumTextures;

	m_Textures.reserve(m_iNumTextures);

	for (size_t i = 0; i < iNumTextures; i++)
	{
		_tchar		szTextureFilePath[MAX_PATH] = TEXT("");
		ID3D11ShaderResourceView* pSRV = { nullptr };

		wsprintf(szTextureFilePath, pTextureFilePath, i);	

		_tchar		szExt[MAX_PATH] = {};	

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		if (false == lstrcmp(szExt, TEXT(".dds")))
		{
			if (FAILED(CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV)))
				return E_FAIL;
		}
		else if (false == lstrcmp(szExt, TEXT(".tga")))
		{
			return E_FAIL;
		}
		else
		{
			if (FAILED(CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV)))
				return E_FAIL;
		}

		m_Textures.push_back(pSRV);	
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_ShaderResource(pConstantName, m_Textures[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, const _char* pConstantName)
{

	return pShader->Bind_ShaderResources(pConstantName, &m_Textures.front(), m_iNumTextures);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_Textures)
		Safe_Release(pSRV);

	m_Textures.clear();
}
