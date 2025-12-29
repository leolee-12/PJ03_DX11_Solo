#include "CFont.h"	//	(P)

CFont::CFont(LPDIRECT3DDEVICE9 pGraphicDev)
	:	m_pGraphicDev(pGraphicDev),
		m_pSprite(nullptr),
		m_pFont(nullptr)
{
	m_pGraphicDev->AddRef();
}

CFont::~CFont()
{
}

HRESULT CFont::Ready_Font(const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	D3DXFONT_DESC			tFont_Desc;	// 폰트 옵션 관련 구조체
	ZeroMemory(&tFont_Desc, sizeof(D3DXFONT_DESC));

	tFont_Desc.CharSet = HANGUL_CHARSET;		// 문자 체계
	tFont_Desc.Width = iWidth;					// 폰트 가로크기
	tFont_Desc.Height = iHeight;				// 폰트 세로크기
	tFont_Desc.Weight = iWeight;				// 폰트 굵기
	lstrcpy(tFont_Desc.FaceName, pFontType);	// FaceName : 폰트 이름

	if (FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &tFont_Desc, &m_pFont)))	// 폰트 생성
	{
		MSG_BOX("Font Create Failed");
		return E_FAIL;
	}

	if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite)))	// 폰트 출력을 위해 Sprite 컴객체 생성
	{
		MSG_BOX("Sprite Create Failed");
		return E_FAIL;
	}


	return S_OK;
}

void CFont::Render_Font(const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color)
{
	RECT rc{ (_long)pPos->x, (_long)pPos->y };

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	// Sprite 컴객체의 Begin/End 호출 필요

	m_pFont->DrawTextW(m_pSprite, pString, lstrlen(pString), &rc, DT_NOCLIP, Color);
	// API와 똑같이 Rect 선언 후 DrawText 함수로 텍스트를 출력, 매개변수로 m_pSprite 넣어준다

	m_pSprite->End();
}

CFont* CFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	CFont* pFont = new CFont(pGraphicDev);

	if (FAILED(pFont->Ready_Font(pFontType, iWidth, iHeight, iWeight)))
	{
		Safe_Release(pFont);
		MSG_BOX("Font Create Failed");
		return nullptr;
	}

	return pFont;
}

void CFont::Free()
{
}
