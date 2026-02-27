#include "stdafx.h"
#include "..\Public\QTE_Hit_UI_Icon.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "QTE_Hit_Situation.h"
#include "QTE_Hit_UI_MovingRing_Icon.h"

CQTE_Hit_UI_Icon::CQTE_Hit_UI_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CQTE_Hit_UI_Icon::CQTE_Hit_UI_Icon(const CQTE_Hit_UI_Icon& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CQTE_Hit_UI_Icon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE_Hit_UI_Icon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CQTE_Hit_UI_Icon::QTE_Hit_UI_ICON_DESC* desc = static_cast<CQTE_Hit_UI_Icon::QTE_Hit_UI_ICON_DESC*>(pArg);

	m_fSizeX = desc->fSizeX;
	m_fSizeY = desc->fSizeY;
	m_fX = desc->fX;
	m_fY = desc->fY;
	m_iTextureNumber = desc->iTextureNumber;
	m_Key = desc->key;
	m_fTimer = desc->fTimer;
	m_pHit_Situation = static_cast<CQTE_Hit_Situation*>(desc->Hit_Situation);

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.9f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	SetActive(true);

	//this는 키 텍스쳐를 가지고 있고
	//나머지 둘은 Moving과 Static Ring을 가지고 있다

	//경과시간과 Timer를 알아야 하니
	CQTE_Hit_UI_MovingRing_Icon::Hit_MovingRing_DESC Desc{};
	Desc.pfElaspedTime = &m_fElaspedTime;
	Desc.pfTimer = &m_fTimer;
	Desc.fX = m_fX;
	Desc.fY = m_fY;
	Desc.fSizeX = m_fSizeX * 3.f;
	Desc.fSizeY = m_fSizeY * 3.f;

	m_pHit_MovingRing_Icon = static_cast<CQTE_Hit_UI_MovingRing_Icon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_QTE_Hit_UI_MovingRing_Icon"), &Desc));

	return S_OK;
}

void CQTE_Hit_UI_Icon::Camera_Update(_float fTimeDelta)
{

}

void CQTE_Hit_UI_Icon::Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	//소속된 링들
	m_pHit_MovingRing_Icon->Update(fTimeDelta);

	m_fElaspedTime += fTimeDelta;

	//경과시간이 다 지났다면
	if (m_fElaspedTime >= m_fTimer)
	{
		m_fElaspedTime = m_fTimer;
		m_currentResult_ID = HIT_RESULT_FAILED;

		m_pHit_Situation->Notify_Faild_Result(this);
		// 아이콘 종료 처리
		Finalize_Icon();
	}
}

void CQTE_Hit_UI_Icon::Late_Update(_float fTimeDelta)
{
	if (!m_bIsActive)
		return;

	m_pHit_MovingRing_Icon->Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CQTE_Hit_UI_Icon::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CQTE_Hit_UI_Icon::RESULT_ID CQTE_Hit_UI_Icon::Send_Input(KEY_ID _Input, _bool isFinal)
{
	// 등급 계산
	Calculate_Result();

	// 아이콘 종료 처리
	Finalize_Icon(isFinal);

	return m_currentResult_ID;
}

void CQTE_Hit_UI_Icon::Calculate_Result()
{
	// 입력 타이밍에 따른 비율 계산
	_float ratio = m_fElaspedTime / m_fTimer;

	// 등급 결정
	if (ratio >= 0.95f && ratio <= 1.0f)
	{
		m_currentResult_ID = HIT_RESULT_PERFECT;
	}
	else if (ratio >= 0.90f && ratio < 0.95f)
	{
		m_currentResult_ID = HIT_RESULT_EXCELLENT;
	}
	else if (ratio >= 0.85f && ratio < 0.90f)
	{
		m_currentResult_ID = HIT_RESULT_GOOD;
	}
	else
	{
		m_currentResult_ID = HIT_RESULT_FAILED;
	}
}

void CQTE_Hit_UI_Icon::Finalize_Icon(_bool isFinal)
{
	// 마지막 아이콘인 경우 상황 객체에 알림
	if (isFinal)
		m_pHit_Situation->Notify_Last_UI_Final_Complete();

	SetActive(false);
}

HRESULT CQTE_Hit_UI_Icon::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_QTE_VtxRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ActionInput"),
		TEXT("Com_Input_Texture"), reinterpret_cast<CComponent**>(&m_pTexture_Key_InputButtonCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CQTE_Hit_UI_Icon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTexture_Key_InputButtonCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNumber)))
		return E_FAIL;

	return S_OK;
}

CQTE_Hit_UI_Icon* CQTE_Hit_UI_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE_Hit_UI_Icon* pInstance = new CQTE_Hit_UI_Icon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CQTE_Hit_UI_Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQTE_Hit_UI_Icon::Clone(void* pArg)
{
	CQTE_Hit_UI_Icon* pInstance = new CQTE_Hit_UI_Icon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CQTE_Hit_UI_Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQTE_Hit_UI_Icon::Free()
{
	Safe_Release(m_pTexture_Key_InputButtonCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pHit_MovingRing_Icon);

	__super::Free();
}
