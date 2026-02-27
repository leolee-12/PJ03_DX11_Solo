#include "Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
    if (nullptr != Find_Font(strFontTag))
        return E_FAIL;

    CCustomFont* pFont = CCustomFont::Create(m_pDevice, m_pContext, pFontFilePath);
    if(nullptr == pFont)
        return E_FAIL;

    m_Fonts.emplace(strFontTag, pFont);

    return S_OK;
}

HRESULT CFont_Manager::Draw(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor)
{
    CCustomFont*        pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    pFont->Draw(strText.c_str(), vPosition, vColor);

    return S_OK;
}

CCustomFont* CFont_Manager::Find_Font(const _wstring& strFontTag)
{
    auto    iter = m_Fonts.find(strFontTag);
    if(iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CFont_Manager(pDevice, pContext);
}

void CFont_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_Fonts)
        Safe_Release(Pair.second);

    m_Fonts.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
