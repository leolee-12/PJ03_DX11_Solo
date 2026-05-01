#pragma once
#include "RenderTarget.h"
#include "Editor_Defines.h"

NS_BEGIN(Editor)

class CVP_RenderTarget final : public CRenderTarget
{
private:
    CVP_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CVP_RenderTarget() = default;

public:
    HRESULT Initialize(ImVec2 vInitialSize);

    HRESULT Begin_SceneRender();
    HRESULT End_SceneRender();

    ID3D11DepthStencilView* Get_DSV()  const { return m_pDSV; }
    const ImVec2& Get_Size() const { return m_vSize; }

private:
    ImVec2 m_vSize = {};

    ID3D11Texture2D* m_pDSTexture = { nullptr };
    ID3D11DepthStencilView* m_pDSV = { nullptr };

    ID3D11RenderTargetView* m_pPrevRTV = { nullptr };
    ID3D11DepthStencilView* m_pPrevDSV = { nullptr };
    D3D11_VIEWPORT          m_tPrevViewport = {};
    _uint                   m_iPrevViewportCount = { 1 };

private:
    HRESULT Create_Resources();
    void    Release_Resources();

public:
    static CVP_RenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vInitialSize);

private:
    virtual void Free() override;
};

NS_END