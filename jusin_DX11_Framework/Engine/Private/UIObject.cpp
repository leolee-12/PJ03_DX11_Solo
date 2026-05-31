#include "UIObject.h"
#include "UIAnimator.h"
#include "UICanvasMath.h"
#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
	: CGameObject{ Prototype }
{
}

void CUIObject::On_ViewportResized(_float2 vNewViewport)
{
	if (vNewViewport.x <= 0.f || vNewViewport.y <= 0.f)
		return;

	m_vActualViewportSize = vNewViewport;
	// resize 시 actual만 갱신 (desing, layout은 그대로)

	Recalculate_RenderTransform();
	Update_UI_Transform();
}

void CUIObject::Set_Center(_float fCenterX, _float fCenterY)
{
	m_fCenterX = fCenterX;
	m_fCenterY = fCenterY;
	Refresh_Layout();
}

void CUIObject::Set_Size(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	Refresh_Layout();
}

_float4 CUIObject::Get_DesignRect() const
{
	return _float4(	m_fResolvedCenterX - m_fSizeX * 0.5f,
					m_fResolvedCenterY - m_fSizeY * 0.5f,
					m_fSizeX,
					m_fSizeY);
}

_float4 CUIObject::Get_RenderRect() const
{
	return _float4(	m_fRenderCenterX - m_fRenderSizeX * 0.5f,
					m_fRenderCenterY - m_fRenderSizeY * 0.5f,
					m_fRenderSizeX,
					m_fRenderSizeY);
}

void CUIObject::Set_Anchor(UI_ANCHOR eAnchor, _float fOffsetX, _float fOffsetY, _bool bUseAnchoredPos)
{
	m_tAnchorDesc.eAnchor = eAnchor;
	m_tAnchorDesc.fOffsetX = fOffsetX;
	m_tAnchorDesc.fOffsetY = fOffsetY;
	m_tAnchorDesc.bUseAnchoredPos = bUseAnchoredPos;
	Refresh_Layout();
}

void CUIObject::Set_AnchorOffset(_float fOffsetX, _float fOffsetY)
{
	m_tAnchorDesc.fOffsetX = fOffsetX;
	m_tAnchorDesc.fOffsetY = fOffsetY;
	Refresh_Layout();
}

void CUIObject::Set_DesignCanvasSize(_float fWidth, _float fHeight)
{
	if (fWidth <= 0.f || fHeight <= 0.f)
		return;

	m_tCanvasDesc.fDesignWidth = fWidth;
	m_tCanvasDesc.fDesignHeight = fHeight;

	Refresh_Layout();
}

void CUIObject::Set_FixedViewportSize(_float fWidth, _float fHeight)
{
	if (fWidth <= 0.f || fHeight <= 0.f)
		return;

	m_bUseFixedViewport = true;
	m_vActualViewportSize = { fWidth, fHeight };

	Recalculate_RenderTransform();
	Update_UI_Transform();
}

void CUIObject::Set_ScalePolicy(UI_SCALE_POLICY ePolicy)
{
	if (ePolicy == UI_SCALE_POLICY::END)
		return;

	m_tCanvasDesc.eScalePolicy = ePolicy;

	Recalculate_RenderTransform();
	Update_UI_Transform();
}

HRESULT CUIObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto pDesc = static_cast<UIOBJECT_DESC*>(pArg);
		m_fCenterX = pDesc->fCenterX;
		m_fCenterY = pDesc->fCenterY;
		m_fSizeX = pDesc->fSizeX;
		m_fSizeY = pDesc->fSizeY;
		m_iZOrder = pDesc->iZOrder;
		m_bVisible = pDesc->bVisible;
		m_tAnchorDesc = pDesc->tAnchorDesc;
		m_tLayoutSlot = pDesc->tLayoutSlot;
		m_pParentUI = pDesc->pParentUI;
		m_tCanvasDesc = pDesc->tCanvasDesc;
		m_fPivotX = pDesc->fPivotX;
		m_fPivotY = pDesc->fPivotY;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Animator()))
		return E_FAIL;

	m_fPivotX = max(0.f, min(1.f, m_fPivotX));
	m_fPivotY = max(0.f, min(1.f, m_fPivotY));

	if (m_tCanvasDesc.fDesignWidth <= 0.f)
		m_tCanvasDesc.fDesignWidth = 1920.f;
	if (m_tCanvasDesc.fDesignHeight <= 0.f)
		m_tCanvasDesc.fDesignHeight = 1080.f;
	if (m_tCanvasDesc.eScalePolicy == UI_SCALE_POLICY::END)
		m_tCanvasDesc.eScalePolicy = UI_SCALE_POLICY::UNIFORM_FIT;

	m_vActualViewportSize = m_pGameInstance->Get_ViewportSize();

	// View 행렬 세팅
	XMStoreFloat4x4(&m_TransformMatrices[ETOUI(D3DTS::VIEW)], XMMatrixIdentity());

	Recalculate_Layout();
	Recalculate_RenderTransform();
	Update_UI_Transform();

	return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
}

void CUIObject::Update(_float fTimeDelta)
{
	if (m_pAnimatorCom)
		m_pAnimatorCom->Tick(fTimeDelta);
}

void CUIObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIObject::Render()
{
	return S_OK;
}

void CUIObject::Refresh_Layout()
{
	Recalculate_Layout();
	Recalculate_RenderTransform();
	Update_UI_Transform();
}

void CUIObject::Recalculate_Layout()
{
	if (m_tAnchorDesc.bUseAnchoredPos)
	{
		const _float2 vResolved = Resolve_AnchorCenter();
		m_fResolvedCenterX = vResolved.x;
		m_fResolvedCenterY = vResolved.y;
	}
	else
	{
		m_fResolvedCenterX = m_fCenterX;
		m_fResolvedCenterY = m_fCenterY;
	}
}

void CUIObject::Recalculate_RenderTransform()
{
	if (!m_bUseFixedViewport)
	{
		const _float2 vViewport = m_pGameInstance->Get_ViewportSize();
		if (vViewport.x > 0.f && vViewport.y > 0.f)
			m_vActualViewportSize = vViewport;
	}

	const _float fDesignWidth = (m_tCanvasDesc.fDesignWidth > 0.f) ? m_tCanvasDesc.fDesignWidth : 1.f;
	const _float fDesignHeight = (m_tCanvasDesc.fDesignHeight > 0.f) ? m_tCanvasDesc.fDesignHeight : 1.f;

	const _float fActualWidth = (m_vActualViewportSize.x > 0.f) ? m_vActualViewportSize.x : fDesignWidth;
	const _float fActualHeight = (m_vActualViewportSize.y > 0.f) ? m_vActualViewportSize.y : fDesignHeight;

	const UI_SCALE_POLICY ePolicy = m_tCanvasDesc.eScalePolicy;

	// canvas scale / offset / render extent 계산
	m_tCanvasTransform = UICanvasMath::Build_UITransform(
		fDesignWidth, fDesignHeight,
		fActualWidth, fActualHeight,
		ePolicy);

	// resolved center(design) -> render center
	const _float2 vRenderCenter = UICanvasMath::Design_To_RenderPoint(
		_float2(m_fResolvedCenterX, m_fResolvedCenterY),
		m_tCanvasTransform,
		ePolicy);
	m_fRenderCenterX = vRenderCenter.x;
	m_fRenderCenterY = vRenderCenter.y;

	// size(design) -> render size (offset 미적용, scale만 반영)
	const _float4 vRenderRect = UICanvasMath::Design_To_RenderRect(
		_float4(0.f, 0.f, m_fSizeX, m_fSizeY),
		m_tCanvasTransform,
		ePolicy);
	m_fRenderSizeX = vRenderRect.z;
	m_fRenderSizeY = vRenderRect.w;

	XMStoreFloat4x4(&m_TransformMatrices[ETOUI(D3DTS::PROJ)],
		XMMatrixOrthographicLH(fActualWidth, fActualHeight, 0.f, 1.f));
}

void CUIObject::Apply_LayoutCenter(_float fCenterX, _float fCenterY)
{
	m_fResolvedCenterX = fCenterX;
	m_fResolvedCenterY = fCenterY;

	Recalculate_RenderTransform();
	Update_UI_Transform();
}

_bool CUIObject::Can_Apply_Tween_Target(UI_TWEEN_TARGET eTarget) const
{
	switch (eTarget)
	{
	case UI_TWEEN_TARGET::SIZE_X:
	case UI_TWEEN_TARGET::SIZE_Y:
	case UI_TWEEN_TARGET::ROTATION:
	case UI_TWEEN_TARGET::POSITION_X:
	case UI_TWEEN_TARGET::POSITION_Y:
	case UI_TWEEN_TARGET::ANCHOR_OFFSET_X:
	case UI_TWEEN_TARGET::ANCHOR_OFFSET_Y:
		return true;

	default:
		return false;
	}
}

HRESULT CUIObject::Apply_Tween_Target(UI_TWEEN_TARGET eTarget, _float fValue)
{
	switch (eTarget)
	{
	case UI_TWEEN_TARGET::SIZE_X:			m_fSizeX = fValue;					Refresh_Layout();		return S_OK;
	case UI_TWEEN_TARGET::SIZE_Y:			m_fSizeY = fValue;					Refresh_Layout();		return S_OK;
	case UI_TWEEN_TARGET::ROTATION:			m_fRotation = fValue;				Update_UI_Transform();	return S_OK;
	case UI_TWEEN_TARGET::POSITION_X:		m_fCenterX = fValue;				Refresh_Layout();		return S_OK;
	case UI_TWEEN_TARGET::POSITION_Y:		m_fCenterY = fValue;				Refresh_Layout();		return S_OK;
	case UI_TWEEN_TARGET::ANCHOR_OFFSET_X:	m_tAnchorDesc.fOffsetX = fValue;	Refresh_Layout();		return S_OK;
	case UI_TWEEN_TARGET::ANCHOR_OFFSET_Y:	m_tAnchorDesc.fOffsetY = fValue;	Refresh_Layout();		return S_OK;
	default: return E_FAIL;
	}
}

HRESULT CUIObject::Ready_Animator()
{
	m_pAnimatorCom = CUIAnimator::Create(m_pDevice, m_pContext);

	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	CUIAnimator::UIANIMATOR_DESC tDesc = {};
	tDesc.pOwner = this;

	if (FAILED(m_pAnimatorCom->Initialize(&tDesc)))
		return E_FAIL;

	m_Components.emplace(COM_UI_ANIMATOR, m_pAnimatorCom);
	Safe_AddRef(m_pAnimatorCom);

	return S_OK;
}

HRESULT CUIObject::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, D3DTS eType)
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformMatrices[ETOUI(eType)]);
}

_float4 CUIObject::Get_ReferenceRect() const
{
	if (m_pParentUI)
		return m_pParentUI->Get_DesignRect();

	return _float4(0.f, 0.f, m_tCanvasDesc.fDesignWidth, m_tCanvasDesc.fDesignHeight);
}

_float2 CUIObject::Resolve_AnchorCenter() const
{
	if (!m_tAnchorDesc.bUseAnchoredPos)
		return _float2(m_fCenterX, m_fCenterY);

	const _float4 vRefRect = Get_ReferenceRect();

	const _float fLeft = vRefRect.x;
	const _float fTop = vRefRect.y;
	const _float fRight = vRefRect.x + vRefRect.z;
	const _float fBottom = vRefRect.y + vRefRect.w;
	const _float fCenterX = vRefRect.x + vRefRect.z * 0.5f;
	const _float fCenterY = vRefRect.y + vRefRect.w * 0.5f;

	_float fAnchorX = fCenterX;
	_float fAnchorY = fCenterY;

	switch (m_tAnchorDesc.eAnchor)
	{
	case UI_ANCHOR::TL: fAnchorX = fLeft;    fAnchorY = fTop;     break;
	case UI_ANCHOR::TC: fAnchorX = fCenterX; fAnchorY = fTop;     break;
	case UI_ANCHOR::TR: fAnchorX = fRight;   fAnchorY = fTop;     break;
	case UI_ANCHOR::ML: fAnchorX = fLeft;    fAnchorY = fCenterY; break;
	case UI_ANCHOR::MC: fAnchorX = fCenterX; fAnchorY = fCenterY; break;
	case UI_ANCHOR::MR: fAnchorX = fRight;   fAnchorY = fCenterY; break;
	case UI_ANCHOR::BL: fAnchorX = fLeft;    fAnchorY = fBottom;  break;
	case UI_ANCHOR::BC: fAnchorX = fCenterX; fAnchorY = fBottom;  break;
	case UI_ANCHOR::BR: fAnchorX = fRight;   fAnchorY = fBottom;  break;
	default: break;
	}

	const _float fDesignLeft = fAnchorX + m_tAnchorDesc.fOffsetX - m_fSizeX * m_fPivotX;
	const _float fDesignTop = fAnchorY + m_tAnchorDesc.fOffsetY - m_fSizeY * m_fPivotY;

	const _float fDesignCenterX = fDesignLeft + m_fSizeX * 0.5f;
	const _float fDesignCenterY = fDesignTop + m_fSizeY * 0.5f;

	return _float2(fDesignCenterX, fDesignCenterY);
}

void CUIObject::Update_UI_Transform()
{
	const _float fActualWidth = (m_vActualViewportSize.x > 0.f) ? m_vActualViewportSize.x : m_tCanvasDesc.fDesignWidth;
	const _float fActualHeight = (m_vActualViewportSize.y > 0.f) ? m_vActualViewportSize.y : m_tCanvasDesc.fDesignHeight;

	m_pTransformCom->ScaleTo(m_fRenderSizeX, m_fRenderSizeY, 1.f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_fRotation);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fRenderCenterX - fActualWidth * 0.5f,
															-m_fRenderCenterY + fActualHeight * 0.5f,
															0.f,
															1.f));
}

void CUIObject::Free()
{
	__super::Free();

	Safe_Release(m_pAnimatorCom);
}
