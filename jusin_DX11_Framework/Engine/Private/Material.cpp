#include "Material.h"
#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMaterial::Initialize(const WMODEL_MATERIAL& tMat, const _char* pBaseDir)
{
	namespace fs = std::filesystem;
	fs::path texPath = {};

	// aiTextureType
	for (size_t i = 0; i < ETOUI(MATERIAL_TYPE::END); i++)
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

	if(FAILED(CreateWICTextureFromFile(m_pDevice, L"../../Resources/dummy/dummy_white.png", nullptr, &m_pDefaultMaterial)))
		return E_FAIL;

	if (FAILED(CreateWICTextureFromFile(m_pDevice, L"../../Resources/dummy/dummy_normal.png", nullptr, &m_pDefaultNormal)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, MATERIAL_TYPE eType, _uint iIndex)
{
	if (eType >= MATERIAL_TYPE::END)
		return E_FAIL;


	if (iIndex >= m_Materials[ETOUI(eType)].size())
	{
		ID3D11ShaderResourceView* pDefaultSRV =
			(MATERIAL_TYPE::NORMALS == eType) ? m_pDefaultNormal : m_pDefaultMaterial;

		return pShader->Bind_SRV(pConstantName, pDefaultSRV);
	}

	return pShader->Bind_SRV(pConstantName, m_Materials[ETOUI(eType)][iIndex]);
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
	for (auto& SRVs : m_Materials)
	{
		for (auto& pSRV : SRVs)
			Safe_Release(pSRV);

		SRVs.clear();
	}

	Safe_Release(m_pDefaultMaterial);
	Safe_Release(m_pDefaultNormal);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}
