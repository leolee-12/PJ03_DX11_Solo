#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGraphic_Device : public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	HRESULT Initialize(HWND hWnd, WINMODE isWindowed, _uint iWinSizeX, _uint iWinSizeY, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);

	HRESULT Clear_BackBuffer_View(const _float4* pClearColor);

	HRESULT Clear_DepthStencil_View();

	HRESULT Present();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CGraphic_Device*	Create(_In_ HWND hWnd, WINMODE isWindowed, _uint iWinSizeX, _uint iWinSizeY, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
	virtual void			Free() override;
};

NS_END