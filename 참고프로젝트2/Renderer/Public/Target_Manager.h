#pragma once

#include "Base.h"
#include "Renderer_Defines.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Renderer)

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor);
	_int Add_ClientRenderTarget(const _wstring& strMRTTag, const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor);
	HRESULT Add_ClientRenderTargetToMRT(const _wstring& strMRTTag, const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT Begin_MRT_DoNotClear(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D);
	ID3D11ShaderResourceView* Copy_RenderTarget_SRV(const _wstring& strTargetTag);
	HRESULT Bind_ShaderResource(class CShader * pShader, const _char * pConstantName, const _wstring& strTargetTag);
	HRESULT Bind_EffectShaderResource(class CShader* pShader, const _char* pConstantName, const _wstring& strTargetTag, _uint isPri);

	HRESULT Begin_EffectMRT(const _wstring& strMRTTag, _uint iArrayIndex = 2 , ID3D11DepthStencilView* pDSV = nullptr);
	void	Init_ArrayCount();

	_int	Get_FrameGlowCount(_int isPri);
	_float	Get_CulGlowIndex(_int isPri);
	void	Zero_CurGlowIndex(_int isPri);
#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& strTargetTag, _float fCenterX, _float fCenterY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif


private:
	ID3D11Device*								m_pDevice = { nullptr };
	ID3D11DeviceContext*						m_pContext = { nullptr };	

	map<const _wstring, class CRenderTarget*>		m_RenderTargets;
	map<const _wstring, list<class CRenderTarget*>>	m_MRTs;

	_uint m_iBeginEffectCountArray[10] = { 0,0,0,0,0,0,0,0,0,0 };
private:
	ID3D11RenderTargetView*						m_pOldRTV = { nullptr };
	ID3D11DepthStencilView*						m_pOldDSV = { nullptr };

public:
	class CRenderTarget* Find_RenderTarget(const _wstring& strTargetTag);
	class list<class CRenderTarget*>* Find_MRT(const _wstring& strMRTTag);

public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
};

END