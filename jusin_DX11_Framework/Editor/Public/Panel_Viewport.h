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

	const ImVec2& Get_CanvasSize()  const { return m_vCanvasSize; }
	const ImVec2& Get_DisplaySize() const { return m_vDisplaySize; }
	const ImVec2& Get_DisplayPos()  const { return m_vDisplayPos; }

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

	ImVec2 m_vCanvasSize = {};	// Game RT/DSV 크기(클라이언트와 동일)
	ImVec2 m_vDisplaySize = {};	// 패널 내부에 그려지는 사각형 크기
	ImVec2 m_vDisplayPos = {};	// 위 사각형의 좌상단 좌표

	// UI 픽킹/드래그 상태 (VPMODE::UI_LAYOUT, UI_ANIM 한정)
	_bool   m_bUIDragging = { false };
	_string m_strDragId = {};
	ImVec2  m_vDragStartMouse = {};
	_bool   m_bDragWasAnchored = { false };
	_float  m_fDragStartCenterX = 0.f;
	_float  m_fDragStartCenterY = 0.f;

	_bool m_bHasLastHit = { false };
	_float3 m_vLastObjectPos = {};
	_float3 m_vLastLocalHitPos = {};
	_float3 m_vLastWorldHitPos = {};
	_string m_strPickDebug = { "No Pick" };
	_string m_strPickTarget = { "Target : None" };
	CGameObject* m_pLastPickedObject = { nullptr };

private:
	HRESULT Create_RenderTarget();
	void Release_RenderTarget();
	void Handle_DebugPicking();
	_bool Build_MouseRay(_float3* pOutOrigin, _float3* pOutDir) const;
	_bool Pick_ModelObject(CGameObject* pObj, CModel* pModel, _fvector vRayOrigin, _fvector vRayDir, _float3* pOutLocalHitPos, _float3* pOutWorldHitPos) const;

	void Handle_ViewportClick();
	void Place_ObjectAtHit(const _float3& vHitPos);
	void Pick_SelectObject();

	void Handle_UIPick();
	void Handle_UIDrag();
	void Draw_UIOverlay();

public:
	static CPanel_Viewport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	virtual void Free() override;
};

NS_END