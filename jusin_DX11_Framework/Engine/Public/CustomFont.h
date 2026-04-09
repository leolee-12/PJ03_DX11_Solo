#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	HRESULT XM_CALLCONV Draw(const _tchar* pText, const _float2& vPosition,
		_fvector vColor, _float fRotation, const _float2& vOrigin, const _float2& vScale);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	SpriteBatch* m_pBatch = { nullptr };
	SpriteFont* m_pFont = { nullptr };

public:
	static CCustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual void Free() override;
};

NS_END