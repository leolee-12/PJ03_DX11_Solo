#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);

	//폰트Tag, 텍스트, 위치(왼쪽정렬), 컬러, 스케일, 회전 중심, 회전각(Radian) 
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fScale, _float2 vOrigin = _float2(0.f, 0.f), _float fRotation = 0.f);

private:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
	map<const wstring, class CCustomFont*>		m_Fonts;

private:
	class CCustomFont* Find_Font(const wstring& strFontTag);

public:
	static CFont_Manager* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END