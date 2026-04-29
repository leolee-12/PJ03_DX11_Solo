#include "UIObject.h"
#include "UIAnimator.h"
#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
	: CGameObject{ Prototype }
{
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

_float4 CUIObject::Get_ScreenRect() const
{
	return _float4(	m_fResolvedCenterX - m_fSizeX * 0.5f,
					m_fResolvedCenterY - m_fSizeY * 0.5f,
					m_fSizeX,
					m_fSizeY);
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
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Animator()))
		return E_FAIL;

	m_vRefSize = m_pGameInstance->Get_OriginRefSize();

	// View 행렬 세팅
	XMStoreFloat4x4(&m_TransformMatrices[ETOUI(D3DTS::VIEW)], XMMatrixIdentity());

	// Proj 행렬 세팅
	XMStoreFloat4x4(&m_TransformMatrices[ETOUI(D3DTS::PROJ)],
		XMMatrixOrthographicLH(m_vRefSize.x, m_vRefSize.y, 0.f, 1.f));

	Refresh_Layout();

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

	Update_UI_Transform();
}

void CUIObject::Apply_LayoutCenter(_float fCenterX, _float fCenterY)
{
	m_fResolvedCenterX = fCenterX;
	m_fResolvedCenterY = fCenterY;
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
	case UI_TWEEN_TARGET::SIZE_X:			m_fSizeX = fValue;					Refresh_Layout(); return S_OK;
	case UI_TWEEN_TARGET::SIZE_Y:			m_fSizeY = fValue;					Refresh_Layout(); return S_OK;
	case UI_TWEEN_TARGET::ROTATION:			m_fRotation = fValue;				Refresh_Layout(); return S_OK;
	case UI_TWEEN_TARGET::POSITION_X:		m_fCenterX = fValue;				Refresh_Layout(); return S_OK;
	case UI_TWEEN_TARGET::POSITION_Y:		m_fCenterY = fValue;				Refresh_Layout(); return S_OK;
	case UI_TWEEN_TARGET::ANCHOR_OFFSET_X:	m_tAnchorDesc.fOffsetX = fValue;	Refresh_Layout(); return S_OK;
	case UI_TWEEN_TARGET::ANCHOR_OFFSET_Y:	m_tAnchorDesc.fOffsetY = fValue;	Refresh_Layout(); return S_OK;
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
		return m_pParentUI->Get_ScreenRect();

	return _float4(0.f, 0.f, m_vRefSize.x, m_vRefSize.y);
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

	return _float2(
		fAnchorX + m_tAnchorDesc.fOffsetX,
		fAnchorY + m_tAnchorDesc.fOffsetY);
}

void CUIObject::Update_UI_Transform()
{
	m_pTransformCom->ScaleTo(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_fRotation);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fResolvedCenterX - m_vRefSize.x * 0.5f,
															-m_fResolvedCenterY + m_vRefSize.y * 0.5f,
															0.f,
															1.f));
}

void CUIObject::Free()
{
	__super::Free();

	Safe_Release(m_pAnimatorCom);
}
