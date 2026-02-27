#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager();
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Draw_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vFontColor, _float fRadian, _float2 vPivotPos, _float fScale);
	HRESULT Draw_FontShadow(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vFontColor, _fvector vShadowFontColor, _float fShadowSize, _float fRadian, _float2 vPivotPos, _float fScale);

private:
	map<const _wstring, class CCustomFont*>			m_Fonts;

private:
	class CCustomFont* Find_Font(const _wstring& strFontTag);

public:
	static CFont_Manager* Create();
	virtual void Free() override;
};

END