#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWidth, _uint iHeight);
	void Update();
	_bool Picking(_float3* pOut);

private:
	HWND		m_hWnd = {};
	ID3D11Device* m_pDevice = {};
	ID3D11DeviceContext* m_pContext = {};

	ID3D11Texture2D* m_pTexture2D = { nullptr };
	_float4* m_pPixelPositions = { nullptr };

	class CGameInstance* m_pGameInstance = { nullptr };
	_uint	m_iNumPixels = { };
	_uint	m_iNumPixelX = { };

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWidth, _uint iHeight);
	virtual void Free() override;
};

NS_END