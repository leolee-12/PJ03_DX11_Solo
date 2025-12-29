#pragma once

#include "Engine_Define.h"
#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CGraphicDev : public CBase
{
	DECLARE_SINGLETON(CGraphicDev)

private:
	explicit CGraphicDev();
	virtual ~CGraphicDev();

public:
	LPDIRECT3DDEVICE9	Get_GraphicDev() { return m_pGraphicDev; }

public:
	HRESULT		Ready_GraphicDev(HWND hWnd, 
								WINMODE eMode,
								const _uint& iSizeX,
								const _uint& iSizeY,
								CGraphicDev** ppGraphicDev);

	void	Render_Begin(D3DXCOLOR Color);
	void	Render_End();

private:
	LPDIRECT3D9			m_pSDK;					// 그래픽 카드를 조사할 객체
	LPDIRECT3DDEVICE9	m_pGraphicDev;			// 조사된 정보를 토대로 각종 그리기 함수를 제공하는 그리기 객체

private:
	virtual void Free();

};

END

