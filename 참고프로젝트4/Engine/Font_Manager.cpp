#include "Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	 
	 
}

HRESULT CFont_Manager::Initialize()
{
	return S_OK;
}


HRESULT CFont_Manager::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	if (nullptr != Find_Font(strFontTag))
		RETURN_EFAIL;

	CCustomFont* pFont = CCustomFont::Create(m_pDevice, m_pContext, strFontFilePath);
	if (nullptr == pFont)
		RETURN_EFAIL;

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	CCustomFont* pFont = Find_Font(strFontTag);

	if (nullptr == pFont)
		RETURN_EFAIL;

	return pFont->Render(strText, vPosition, vColor, fScale, vOrigin, fRotation);
}

CCustomFont* CFont_Manager::Find_Font(const wstring& strFontTag)
{
	auto	iter = m_Fonts.find(strFontTag);

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;
}

CFont_Manager* CFont_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CFont_Manager* pInstance = new CFont_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFont_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();

	 
	 
}