#include "Material.h"
#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMaterial::Intialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	/* 머테리얼에 정의되어있는 텍스쳐정보(경로)를 얻어온다.*/
	aiString			strTextureFilePath{};

	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint			iNumTextures = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));

		for (size_t j = 0; j < iNumTextures; j++)
		{
			_char			szDrive[MAX_PATH] = {};
			_char			szDir[MAX_PATH] = {};
			_char			szFileName[MAX_PATH] = {};
			_char			szExt[MAX_PATH] = {};


			pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTextureFilePath);

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			_char			szFullPath[MAX_PATH] = {};

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar			szWFullPath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWFullPath, MAX_PATH);

			HRESULT				hr = {};

			ID3D11ShaderResourceView* pSRV = { nullptr };

			if (false == strcmp(".dds", szExt))
				hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szWFullPath, nullptr, &pSRV);
			else if (false == strcmp(".tga", szExt))
				hr = E_FAIL;
			else
				hr = DirectX::CreateWICTextureFromFile(m_pDevice, szWFullPath, nullptr, &pSRV);

			if (FAILED(hr))
				return E_FAIL;

			m_Textures[i].push_back(pSRV);
		}	
	}

	

	return S_OK;
}

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iIndex)
{
	if (iIndex >= m_Textures[eType].size())
		return E_FAIL;

	return pShader->Bind_ShaderResource(pConstantName, m_Textures[eType][iIndex]);	
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Intialize(pModelFilePath, pAIMaterial)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMaterial::Free()
{
	__super::Free();

	for (auto& Textures : m_Textures)
	{
		for (auto& pSRV : Textures)
			Safe_Release(pSRV);

		Textures.clear();
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
