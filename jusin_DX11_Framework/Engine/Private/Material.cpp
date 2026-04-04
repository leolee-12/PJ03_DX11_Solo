#include "Material.h"
#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMaterial::Initialize(aiMaterial* pAIMaterial, const _char* pModelFilePath)
{
	_char szDrive[MAX_PATH] = {};
	_char szDir[MAX_PATH] = {};

	_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

	// aiTextureType
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint	iNumTextures = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));

		for (_uint j = 0; j < iNumTextures; j++)
		{
			aiString strTexturePath = {};
			
			_char szFileName[MAX_PATH] = {};
			_char szExt[MAX_PATH] = {};

			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath)))
				return E_FAIL;

			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			_char szFullPath[MAX_PATH] = {};
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar szFinalPath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, static_cast<_int>(strlen(szFullPath)), szFinalPath, MAX_PATH);

			HRESULT hr = {};

			ID3D11ShaderResourceView* pSRV = { nullptr };

			if (false == strcmp(szExt, ".dds"))
			{
				hr = CreateDDSTextureFromFile(m_pDevice, szFinalPath, nullptr, &pSRV);
			}
			else if (false == strcmp(szExt, ".tga"))
			{
				hr = E_FAIL;
			}
			else
			{
				hr = CreateWICTextureFromFile(m_pDevice, szFinalPath, nullptr, &pSRV);
			}

			if (FAILED(hr))
				return E_FAIL;

			m_Materials[i].push_back(pSRV);
		}
	}

	return S_OK;
}

HRESULT CMaterial::Initialize_Ex(aiMaterial* pAIMaterial, const _char* pModelFilePath)
{
	namespace fs = std::filesystem;
	
	aiString strTexturePath = {};
	fs::path modelDir = fs::path(pModelFilePath).parent_path();
	fs::path texPath = {};

	// aiTextureType
	for (size_t i = 0; i < ETOUI(TEXTURE_TYPE::END); i++)
	{
		_uint iNumTextures = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));

		for (_uint j = 0; j < iNumTextures; j++)
		{
			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath)))
				return E_FAIL;

			texPath = modelDir / fs::path(strTexturePath.data).filename();
			_wstring wStrFullPath = texPath.wstring();
			_wstring wStrExt = texPath.extension().wstring();

			HRESULT hr = {};
			ID3D11ShaderResourceView* pSRV = { nullptr };

			if (false == lstrcmpW(wStrExt.c_str(), L".dds"))
			{
				hr = CreateDDSTextureFromFile(m_pDevice, wStrFullPath.c_str(), nullptr, &pSRV);
			}
			else if (false == lstrcmpW(wStrExt.c_str(), L".tga"))
			{
				hr = E_FAIL;
			}
			else
			{
				hr = CreateWICTextureFromFile(m_pDevice, wStrFullPath.c_str(), nullptr, &pSRV);
			}

			if (FAILED(hr))
				return E_FAIL;

			m_Materials[i].push_back(pSRV);
		}
	}

	return S_OK;
}

HRESULT CMaterial::Initialize(const WMODEL_MATERIAL& tMat, const _char* pBaseDir)
{
	namespace fs = std::filesystem;
	fs::path texPath = {};

	// aiTextureType
	for (size_t i = 0; i < ETOUI(TEXTURE_TYPE::END); i++)
	{
		_uint iNumTextures = static_cast<_uint>(tMat.TexturePaths[i].size());

		for (_uint j = 0; j < iNumTextures; j++)
		{
			texPath = pBaseDir / fs::path(tMat.TexturePaths[i][j]).filename();
			_wstring wStrFullPath = texPath.wstring();
			_wstring wStrExt = texPath.extension().wstring();

			HRESULT hr = {};
			ID3D11ShaderResourceView* pSRV = { nullptr };

			if (false == lstrcmpW(wStrExt.c_str(), L".dds"))
			{
				hr = CreateDDSTextureFromFile(m_pDevice, wStrFullPath.c_str(), nullptr, &pSRV);
			}
			else if (false == lstrcmpW(wStrExt.c_str(), L".tga"))
			{
				hr = E_FAIL;
			}
			else
			{
				hr = CreateWICTextureFromFile(m_pDevice, wStrFullPath.c_str(), nullptr, &pSRV);
			}

			if (FAILED(hr))
				return E_FAIL;

			m_Materials[i].push_back(pSRV);
		}
	}

	return S_OK;
}

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, TEXTURE_TYPE eType, _uint iIndex)
{
	if (eType >= TEXTURE_TYPE::END ||
		iIndex >= m_Materials[ETOUI(eType)].size())
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_Materials[ETOUI(eType)][iIndex]);
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, aiMaterial* pAIMaterial, const _char* pModelFilePath)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Ex(pAIMaterial, pModelFilePath)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WMODEL_MATERIAL& tMat, const _char* pBaseDir)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(tMat, pBaseDir)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMaterial::Free()
{
	__super::Free();

	for (auto& SRVs : m_Materials)
	{
		for (auto& pSRV : SRVs)
			Safe_Release(pSRV);

		SRVs.clear();
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
