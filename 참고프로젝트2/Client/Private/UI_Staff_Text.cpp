#include "stdafx.h"

#include "UI_Staff_Text.h"

#include "GameInstance.h"
#include "RenderInstance.h"

CUI_Staff_Text::CUI_Staff_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice ,pContext }
{
}

CUI_Staff_Text::CUI_Staff_Text(const CUI_Staff_Text& Prototype)
	:CGameObject{ Prototype }
{
}

HRESULT CUI_Staff_Text::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Staff_Text::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Add_Text();

	STAFF_TEXT_DESC* pDesc = static_cast<STAFF_TEXT_DESC*>(pArg);
	m_pStaffTransform = pDesc->pStaffTransform;
	Safe_AddRef(m_pStaffTransform);

	_vector vStaffPos = m_pStaffTransform->Get_State(CTransform::STATE_POSITION);
	//XMVectorSetY(vStaffPos , XMVectorGetY(vStaffPos) + )
	
	//m_pTransformCom->Set_State(m_pStaffTransform->Get)

	return S_OK;
}

void CUI_Staff_Text::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Staff_Text::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_vTextArray.empty() == FALSE && m_fOffsetPosY >= 150)
		TextOutput(0.025f, fTimeDelta);

	if (m_bIsActive == FALSE && m_vTextArray.empty())
	{
		Add_Text();
		m_bFinishText = TRUE;
	}
}

void CUI_Staff_Text::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Staff_Text::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGameInstance->Draw_Font(TEXT("Font_Nexon"),
		m_strOutputText.c_str(),
		_float2(200.f * 1.5f, 590.f * 1.5f),
		_vector{ 0.f,0.f,0.f,1.f },
		0.f,
		{ 0.f, 0.f },
		0.65f
	);

	return S_OK;
}

HRESULT CUI_Staff_Text::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_VtxRect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_UI_Lobby_StaffTextBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Staff_Text::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

void CUI_Staff_Text::Add_Text()
{
	m_vTextArray.push( TEXT("안녕하세요! 오늘 하루도 잘 보내고 계신가요?"));
	m_vTextArray.push( TEXT("좋은 아침이에요! 기분 좋게 하루를 시작하시길 바랍니다."));
	m_vTextArray.push( TEXT("오랜만이에요! 요즘 어떻게 지내셨나요?"));
}

void CUI_Staff_Text::TextOutput(_float fOutputSpeed, _float fTimeDelta)
{
	m_fFontDration += fTimeDelta;

	_bool bFontFinish = (m_vTextArray.front().length() == m_iTextIndex);

	if (m_fFontDration >= fOutputSpeed && bFontFinish == FALSE)
	{
		m_strOutputText += m_vTextArray.front().at(m_iTextIndex);
		m_iTextIndex++;
		m_fFontDration = 0.f;
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextCharaIcon"))->SetActive(TRUE);
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextCursor"))->SetActive(TRUE);

	}

	if (bFontFinish && m_pGameInstance->Key_Down(DIK_RETURN))
	{
		m_iTextIndex = 0;
		m_strOutputText.clear();
		m_vTextArray.pop();
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextCharaIcon"))->SetActive(FALSE);
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextCursor"))->SetActive(FALSE);
	}
}

CUI_Staff_Text* CUI_Staff_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Staff_Text* pInstatnce = new CUI_Staff_Text(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Staff_Text"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Staff_Text::Clone(void* pArg)
{
	CUI_Staff_Text* pInstatnce = new CUI_Staff_Text(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Staff_Text"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Staff_Text::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pStaffTransform);

	__super::Free();
}
