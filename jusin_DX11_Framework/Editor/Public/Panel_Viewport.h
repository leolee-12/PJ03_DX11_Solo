#pragma once
#include "Panel_Base.h"

NS_BEGIN(Engine)
class CGameObject;
class CModel;
NS_END

NS_BEGIN(Editor)

class CPanel_Viewport final : public CPanel_Base
{
private:
	CPanel_Viewport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPanel_Viewport() = default;

public:
	ID3D11RenderTargetView* Get_RenderTargetView() const { return m_pRTV; }
	ID3D11DepthStencilView* Get_DepthStencilView() const { return m_pDSV; }
	ID3D11ShaderResourceView* Get_ShaderResourceView() const { return m_pSRV; }

	const ImVec2& Get_PanelRTSize() const { return m_vPanelRTSize; }
	const ImVec2& Get_PanelPTPos() const { return m_vPanelRTPos; }

	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT Begin_SceneRender();
	HRESULT End_SceneRender();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	ID3D11Texture2D* m_pRTTexture = { nullptr };
	ID3D11RenderTargetView* m_pRTV = { nullptr };
	ID3D11ShaderResourceView* m_pSRV = { nullptr };

	ID3D11Texture2D* m_pDSTexture = { nullptr };
	ID3D11DepthStencilView* m_pDSV = { nullptr };

	ID3D11RenderTargetView* m_pPrevRTV = { nullptr };
	ID3D11DepthStencilView* m_pPrevDSV = { nullptr };
	D3D11_VIEWPORT m_tPrevViewport = {};
	_uint m_iPrevViewportCount = { 1 };

	_float2 m_vRefSize = {};
	ImVec2 m_vPanelRTSize = {};
	ImVec2 m_vPanelRTPos = {};

	_bool m_bHasLastHit = { false };
	_float3 m_vLastObjectPos = {};
	_float3 m_vLastLocalHitPos = {};
	_float3 m_vLastWorldHitPos = {};
	_string m_strPickDebug = { "No Pick" };
	_string m_strPickTarget = { "Target : None" };
	CGameObject* m_pLastPickedObject = { nullptr };

	// drag ป๓ลย
	_bool m_bUIDragging = { false };
	_string m_strDragId;
	ImVec2 m_vDragStartMouse = {};
	_float m_fDragStartCenterX = { 0.f };
	_float m_fDragStartCenterY = { 0.f };
	_bool m_bDragWasAnchored = { false };

private:
	HRESULT Create_RenderTarget(_uint iWidth, _uint iHeight);
	void Release_RenderTarget();
	void Handle_DebugPicking();
	_bool Build_MouseRay(_float3* pOutOrigin, _float3* pOutDir) const;
	_bool Pick_ModelObject(CGameObject* pObj, CModel* pModel, _fvector vRayOrigin, _fvector vRayDir, _float3* pOutLocalHitPos, _float3* pOutWorldHitPos) const;

	void Handle_ViewportClick();
	void Place_ObjectAtHit(const _float3& vHitPos);
	void Pick_SelectObject();

	void Draw_UIOverlay();
	void Handle_UIPick();
	void Handle_UIDrag();

public:
	static CPanel_Viewport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	virtual void Free() override;
};

NS_END