#pragma once

#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CCustomFont() = default;
public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	HRESULT Draw(const _tchar * pText, const _float2 & vPosition, _fvector vFontColor, _float fRadian, _float2 vPivotPos, _float fScale);
	HRESULT Draw_FontShadow(const _tchar* pText, const _float2& vPosition, _fvector vFontColor, _fvector vShadowFontColor, _float fShadowSize, _float fRadian, _float2 vPivotPos, _float fScale);


private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	ID3D11BlendState* m_pBlendState = { nullptr };

private:
	SpriteFont*				m_pFont = { nullptr };
	SpriteBatch*			m_pBatch = { nullptr };

public:
	static CCustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual void Free() override;
};

END