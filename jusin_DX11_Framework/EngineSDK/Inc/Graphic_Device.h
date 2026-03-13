#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGraphic_Device : public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	HRESULT Initialize(	HWND hWnd, WINMODE isWindowed, _uint iWinSizeX, _uint iWinSizeY,
						_Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);

	HRESULT Clear_BackBuffer_View(const _float4* pClearColor);	// 백버퍼 지우기
	HRESULT Clear_DepthStencil_View();							// 깊이버퍼 + 스텐실버퍼 지우기
	HRESULT Present();											// 백버퍼를 프론트버퍼로 교체 (백버퍼를 화면에 보여준다)
	ID3D11RenderTargetView** Get_BackBufferRTV() { return &m_pBackBufferRTV; }

private:
	ID3D11Device*			m_pDevice = { nullptr };			// COM객체 생성 관련
	ID3D11DeviceContext*	m_pContext = { nullptr };			// COM객체 기능 관련
	IDXGISwapChain*			m_pSwapChain = { nullptr };			// 더블버퍼링용 SwapChain객체
	ID3D11RenderTargetView*	m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView*	m_pDepthStencilView = { nullptr };

private:
	HRESULT Ready_SwapChain(HWND hWnd, WINMODE isWindowed, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilView(_uint iWinCX, _uint iWinCY);

public:
	static CGraphic_Device*	Create(	_In_ HWND hWnd, WINMODE isWindowed, _uint iWinSizeX, _uint iWinSizeY,
									_Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
protected:
	virtual void	Free() override;
};

NS_END