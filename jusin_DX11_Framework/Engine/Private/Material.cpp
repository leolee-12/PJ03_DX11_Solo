#include "Material.h"
#include "Build_Mode.h"
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

	// 더미 SRV 먼저 준비 - 로딩 실패 시 폴백으로 사용
	if (FAILED(CreateWICTextureFromFile(m_pDevice, L"../../Resources/dummy/dummy_white.png", nullptr, &m_pDefaultMaterial)))
		return E_FAIL;

	if (FAILED(CreateWICTextureFromFile(m_pDevice, L"../../Resources/dummy/dummy_normal.png", nullptr, &m_pDefaultNormal)))
		return E_FAIL;

	fs::path texPath = {};

	// aiTextureType
	for (size_t i = 0; i < ETOUI(MATERIAL_TYPE::END); i++)
	{
		_uint iNumTextures = static_cast<_uint>(tMat.TexturePaths[i].size());

		for (_uint j = 0; j < iNumTextures; j++)
		{
			texPath = pBaseDir / fs::path(tMat.TexturePaths[i][j]).filename();
			_wstring wStrFullPath = texPath.wstring();

			HRESULT hr = {};
			ID3D11ShaderResourceView* pSRV = { nullptr };

#if USE_DDS_MATERIAL
			hr = Load_SRV_FromDDS(m_pDevice, wStrFullPath, &pSRV);
#else
			hr = Load_SRV_FromOriginal(m_pDevice, wStrFullPath, &pSRV);
#endif

			if (FAILED(hr))
			{
				// 로딩 실패 -> 더미 폴백 (인덱스 정합성 유지)
				pSRV = (i == ETOUI(MATERIAL_TYPE::NORMALS)) ? m_pDefaultNormal : m_pDefaultMaterial;
				Safe_AddRef(pSRV);
			}

			m_Materials[i].push_back(pSRV);
		}
	}

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

HRESULT CMaterial::Load_SRV_ByExtension(ID3D11Device* pDevice, const _wstring& fullPath, ID3D11ShaderResourceView** ppSRV)
{
	namespace fs = std::filesystem;
	_wstring wStrExt = fs::path(fullPath).extension().wstring();

	if (false == lstrcmpW(wStrExt.c_str(), L".dds"))
		return CreateDDSTextureFromFile(pDevice, fullPath.c_str(), nullptr, ppSRV);

	if (false == lstrcmpW(wStrExt.c_str(), L".tga"))
		return E_FAIL;

	return CreateWICTextureFromFile(pDevice, fullPath.c_str(), nullptr, ppSRV);
}

HRESULT CMaterial::Load_SRV_FromOriginal(ID3D11Device* pDevice, const _wstring& origPath, ID3D11ShaderResourceView** ppSRV)
{
	return Load_SRV_ByExtension(pDevice, origPath, ppSRV);
}

HRESULT CMaterial::Load_SRV_FromDDS(ID3D11Device* pDevice, const _wstring& origPath, ID3D11ShaderResourceView** ppSRV)
{
	return Load_SRV_ByExtension(pDevice, Convert_PathToDDS(origPath), ppSRV);
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
