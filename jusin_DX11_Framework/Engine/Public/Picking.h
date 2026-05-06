#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd);
	void Update();
	_bool Picking(_float4& Out);

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	HWND					m_hWnd;
	ID3D11Texture2D*		m_pTexture2D = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };
	_float4*				m_pWorldPos = { nullptr };

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
	virtual void Free();
};

NS_END