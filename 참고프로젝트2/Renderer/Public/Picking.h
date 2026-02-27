#pragma once

#include "Base.h"
#include "Renderer_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Renderer)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd);
	_float4 Picked_Position(_bool* pPicked);

	_int Picked_Effect_Index();
	// _float4 Picked_Position(const _float4& vWorldPos, _bool* pPicked);


private:
	class CRenderInstance* m_pRenderInstance = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	ID3D11Texture2D*				m_pTexture2D = { nullptr };
	HWND							m_hWnd = {};
	_uint							m_iWidth, m_iHeight;

public:
	static CPicking* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
	virtual void Free() override;
};

END