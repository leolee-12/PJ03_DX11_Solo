#pragma once	//	(P)
#include "CBase.h"
#include "Engine_Define.h"
#include "CFont.h"

// 폰트매니저
// - 폰트도 2D 텍스처처럼 인식, 윈도우에서 제공하는 기본 폰트 사용 가능
// - 외부 폰트도 설치하여 사용 가능하나, 가상 설치 및 웹 사용용 폰트는 적용 불가

BEGIN(Engine)

class ENGINE_DLL CFontMgr : public CBase
{
	DECLARE_SINGLETON(CFontMgr)

private:
	explicit CFontMgr();
	virtual ~CFontMgr();

public:
	HRESULT		Ready_Font(	LPDIRECT3DDEVICE9 pGraphicDev,
							const _tchar* pFontTag,
							const _tchar* pFontType,
							const _uint& iWidth,
							const _uint& iHeight,
							const _uint& iWeight);

	void		Render_Font(const _tchar* pFontTag,
							const _tchar* pString,
							const _vec2* pPos,
							D3DXCOLOR Color);

private:
	CFont*		Find_Font(const _tchar* pFontTag);

private:
	map<const _tchar*, CFont*>	m_mapFont;

private:
	void	Free() override;
};

END