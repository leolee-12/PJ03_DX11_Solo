#include "stdafx.h"

#include "UI_Loading_FlyEff.h"
#include "RenderInstance.h"

CUI_Loading_FlyEff::CUI_Loading_FlyEff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Loading_FlyEff::CUI_Loading_FlyEff(const CUI_Loading_FlyEff& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Loading_FlyEff::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_FlyEff::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_FLYEFF_DESC* pDesc = static_cast<UI_FLYEFF_DESC*>(pArg);
	ThreadID = pDesc->eTheadID;

	m_pHoleTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_LOADING, TEXT("Layer_UI_LoadingHole"), TEXT("Com_Transform") , ThreadID));
	Safe_AddRef(m_pHoleTransform);

	m_iRandomMove = rand() % 2;

	m_fSizeY = 20.f;
	m_fPosX = 730;
	m_fPosY = 170;

	_float fOffsetX = g_iWinSizeX * 0.5f;
	_float fOffsetY = g_iWinSizeY * 0.5f;


	_float svHolePosX = XMVectorGetX(m_pHoleTransform->Get_State(CTransform::STATE_POSITION));
	_float svHolePosY = XMVectorGetY(m_pHoleTransform->Get_State(CTransform::STATE_POSITION));

	_vector vSetPos = {};


	if (m_iRandomMove == 0)
	{
		m_fSizeX = 20.f;

		vSetPos = XMVectorSetX(m_pHoleTransform->Get_State(CTransform::STATE_POSITION), svHolePosX - 40.f);
		vSetPos = XMVectorSetY(vSetPos, svHolePosY + 50.f);
		QueueAnim.push({ 500.f * m_vOffSetWinSize.x - fOffsetX ,(-170.f * m_vOffSetWinSize.y) + fOffsetY, 0.f });
	}
	else
	{
		m_fSizeX = -20.f;

		vSetPos = XMVectorSetX(m_pHoleTransform->Get_State(CTransform::STATE_POSITION), svHolePosX + 40.f);
		vSetPos = XMVectorSetY(vSetPos, svHolePosY + 50.f);
		QueueAnim.push({ 960.f * m_vOffSetWinSize.x - fOffsetX ,(-170.f * m_vOffSetWinSize.y) + fOffsetY, 0.f });
	}

	

	QueueAnim.push(vSetPos);
	QueueAnim.push(m_pHoleTransform->Get_State(CTransform::STATE_POSITION));
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_Loading_FlyEff::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Loading_FlyEff::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	AnimSpeed(fTimeDelta);


	QueueAnim.empty() ? m_bEndAnim = TRUE : Animation(QueueAnim.front(), fTimeDelta);
}

void CUI_Loading_FlyEff::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	RENDER_OBJECT tDesc{};
	tDesc.tGlowDesc.iPassIndex = 2;
	tDesc.tGlowDesc.fGlowFactor = 1.5f;

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI_GLOW, this,&tDesc);
}

HRESULT CUI_Loading_FlyEff::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_FlyEff::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingBallEff"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Loading_FlyEff::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_fAnimFream)))
		return E_FAIL;

	return S_OK;
}

_bool CUI_Loading_FlyEff::Go_Target(_vector fTargetPos ,_float fTimeDelta)
{
	_vector vTargetPos = fTargetPos;
	_vector vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDistance = vTargetPos - vOriginPos;
	_vector vDir = XMVector3Normalize(vDistance);

	_vector MovePos = vOriginPos + vDir * m_fSpeedVaule * fTimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, MovePos);

	m_fScaleOffset += fTimeDelta * 4.f;

	if (m_iRandomMove == 0)
		m_pTransformCom->Set_Scaled(m_fSizeX * m_fScaleOffset, m_fSizeY * m_fScaleOffset,1.f);
	else 
		m_pTransformCom->Set_Scaled(-m_fSizeX * m_fScaleOffset, m_fSizeY * m_fScaleOffset, 1.f);

	_float fLength = XMVectorGetX(XMVector2Length(vDistance));

	_float fDestroyPos = { 0.f };

	(m_bEndAnim) ? fDestroyPos = 100.f : fDestroyPos = 10.f;

	if (fLength <= fDestroyPos)
	{
		QueueAnim.pop();

		if (m_bEndAnim)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetPos);

		return TRUE;
	}

	return FALSE;
}

void CUI_Loading_FlyEff::Animation(_vector fTargetPos, _float fTimeDelta)
{
	//Set_Scaled();
	Go_Target(fTargetPos, fTimeDelta);
}

void CUI_Loading_FlyEff::AnimSpeed(_float fTimeDelta)
{
	switch ((_uint)m_fAnimFream)
	{
	case 0:
		if (QueueAnim.size() == 3)
			m_fAnimFream += fTimeDelta * 2.f;

		m_fSpeedVaule = 500;
		break;

	case 1:		
		m_fAnimFream += fTimeDelta * 4.f;
		break;

	case 2:
		m_fAnimFream += fTimeDelta * 16.f;
		break;

	case 3:
		m_bEndAnim = TRUE;
		m_fSpeedVaule = 1000.f;
		
		if (QueueAnim.size() == 1)
		{
			++m_fAnimFream;
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::LOADING_BALL_SFX, false, 0.2f);
		}
		break;

	case 4:
		m_bEndAnim = FALSE;
		m_fAnimFream += fTimeDelta * 16.f;
		break;

	case 5:
		m_fAnimFream += fTimeDelta * 16.f;
		break;

	case 6:
		m_fAnimFream += fTimeDelta * 16.f;
		break;


	case 7:
		m_bCreateBall = TRUE;
		m_fAnimFream += fTimeDelta * 16.f;
		
		break;

	case 8:
		m_fAnimFream = 8.f;
		Destory();

		break;

	}
}

CUI_Loading_FlyEff* CUI_Loading_FlyEff::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Loading_FlyEff* pInstatnce = new CUI_Loading_FlyEff(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Loading_FlyEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Loading_FlyEff::Clone(void* pArg)
{
	CUI_Loading_FlyEff* pInstatnce = new CUI_Loading_FlyEff(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Loading_FlyEff"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Loading_FlyEff::Free()
{
	Safe_Release(m_pHoleTransform);

	__super::Free();
}
