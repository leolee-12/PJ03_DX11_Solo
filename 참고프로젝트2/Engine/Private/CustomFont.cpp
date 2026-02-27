#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar * pFontFilePath)
{
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);
	m_pBatch = new SpriteBatch(m_pContext);

	D3D11_BLEND_DESC			BlendDesc{};
	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;


	m_pDevice->CreateBlendState(&BlendDesc, &m_pBlendState);

	return S_OK;
}

HRESULT CCustomFont::Draw(const _tchar * pText, const _float2& vPosition, _fvector vFontColor, _float fRadian, _float2 vPivotPos, _float fScale)
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin(DirectX::SpriteSortMode_Deferred, m_pBlendState);

	m_pFont->DrawString(m_pBatch, pText, vPosition, vFontColor, fRadian, vPivotPos, fScale);

	m_pBatch->End();

	return S_OK;
}

HRESULT CCustomFont::Draw_FontShadow(const _tchar* pText, const _float2& vPosition, _fvector vFontColor, _fvector vShadowFontColor, _float fShadowSize, _float fRadian, _float2 vPivotPos, _float fScale)
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);


	if (XMVectorGetW(vFontColor) != 1)
		m_pBatch->Begin(DirectX::SpriteSortMode_Deferred, m_pBlendState);

	else
		m_pBatch->Begin();


	float angleStep = XM_PI / 8.0f; // 16방향으로 렌더링
	for (int i = 0; i < 16; ++i) {
		float angle = angleStep * i;
		float offsetX = fShadowSize * cos(angle);
		float offsetY = fShadowSize * sin(angle);
		m_pFont->DrawString(m_pBatch, pText, XMFLOAT2(vPosition.x + offsetX, vPosition.y + offsetY), vShadowFontColor, fRadian, vPivotPos, fScale);
	}


	m_pFont->DrawString(m_pBatch, pText, vPosition, vFontColor, fRadian, vPivotPos, fScale);

	m_pBatch->End();

	return S_OK;
}


CCustomFont * CCustomFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontFilePath)
{
	CCustomFont*		pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CCustomFont"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomFont::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pBlendState);
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);	
}
