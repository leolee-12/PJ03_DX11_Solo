#include "Texture.h"
#include "Build_Mode.h"
#include "GameInstance.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTexture::CTexture(const CTexture& Prototype)
	: CComponent{ Prototype }
	, m_iNumTextures{ Prototype.m_iNumTextures }
	, m_Textures{ Prototype.m_Textures }
{
	for (auto& pTexture : m_Textures)
		Safe_AddRef(pTexture);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
    m_iNumTextures = iNumTextures;

    // 확장자별 SRV 로딩 - 본 분기와 폴백 분기 둘에서 사용
    auto LoadByExt = [this](const _tchar* pPath, ID3D11ShaderResourceView** ppSRV) -> HRESULT
        {
            _tchar szEXT[MAX_PATH] = { };
            _wsplitpath_s(pPath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

            if (false == lstrcmp(szEXT, TEXT(".dds")))
                return CreateDDSTextureFromFile(m_pDevice, pPath, nullptr, ppSRV);
            if (false == lstrcmp(szEXT, TEXT(".tga")))
                return E_FAIL;
            return CreateWICTextureFromFile(m_pDevice, pPath, nullptr, ppSRV);
        };

    // DDS 모드 토글 - 패턴 자체를 dds 폴더 경로로 변환
    // 가드: 이미 .dds 패턴이면 Convert_PathToDDS 내부에서 그대로 반환
    const _tchar* pUsePattern = pTextureFilePath;
#if USE_DDS_MATERIAL
    _wstring wConverted = Convert_PathToDDS(pTextureFilePath);
    pUsePattern = wConverted.c_str();
#endif

    _tchar szTextureFilePath[MAX_PATH] = TEXT("");

    for (size_t i = 0; i < iNumTextures; i++)
    {
        ID3D11ShaderResourceView* pSRV = { nullptr };
        wsprintf(szTextureFilePath, pUsePattern, i);

        HRESULT hr = LoadByExt(szTextureFilePath, &pSRV);

#if USE_DDS_MATERIAL
        // dds 시도 실패 -> 원본 경로로 폴백 (점진적 변환 지원)
        if (FAILED(hr) && pUsePattern != pTextureFilePath)
        {
            _tchar szOriginalPath[MAX_PATH] = TEXT("");
            wsprintf(szOriginalPath, pTextureFilePath, i);

#ifdef _DEBUG
            _tchar szLog[MAX_PATH * 3] = TEXT("");
            swprintf_s(szLog, TEXT("[CTexture] DDS fallback: %s -> %s (hr=0x%08X)\n"),
                szTextureFilePath, szOriginalPath, static_cast<unsigned int>(hr));
            OutputDebugString(szLog);
#endif
            hr = LoadByExt(szOriginalPath, &pSRV);
        }
#endif

        if (FAILED(hr))
            return E_FAIL;

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

	return pShader->Bind_SRV(pConstantName, m_Textures[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRVs(pConstantName, &m_Textures.front(), m_iNumTextures);
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

	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);

	m_Textures.clear();
}
