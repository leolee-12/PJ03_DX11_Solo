#include "Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Add_Font(const WNameID strFontTag, const _tchar* pFontFilePath)
{
	if (nullptr != Find_Font(strFontTag))
		return E_FAIL;

	CCustomFont* pFont = CCustomFont::Create(m_pDevice, m_pContext, pFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

_float2 CFont_Manager::Measure_Text(const WNameID strFontTag, const _tchar* pText)
{
	CCustomFont* pFont = Find_Font(strFontTag);
	if (nullptr == pFont)
		return _float2(0.f, 0.f);

	return pFont->Measure_Text(pText);
}

HRESULT XM_CALLCONV CFont_Manager::Draw(const WNameID strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, const _float2& vScale)
{
	CCustomFont* pFont = Find_Font(strFontTag);
	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Draw(pText, vPosition, vColor, fRotation, vOrigin, vScale);
}

CCustomFont* CFont_Manager::Find_Font(const WNameID strFontTag)
{
	auto pp = m_Fonts.find(strFontTag);

	return (pp ? *pp : nullptr);
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CFont_Manager(pDevice, pContext);
}

void CFont_Manager::Free()
{
	__super::Free();
	
	m_Fonts.for_each([](auto& Pair)
		{
			Safe_Release(Pair.second);
		});

	m_Fonts.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
