#include "stdafx.h"

#include "UI_Lobby_Text.h"
#include "RenderInstance.h"

CUI_Lobby_Text::CUI_Lobby_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_Lobby_Text::CUI_Lobby_Text(const CUI_Lobby_Text& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Lobby_Text::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Lobby_Text::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//Add_Text();

	m_bIsActive = FALSE;
	m_fSizeX = 960.f, m_fSizeY = 100.f;
	m_fPosX = m_vPrevWinSize.x * 0.5f, m_fPosY = (m_vPrevWinSize.y + 100.f)- m_fSizeY * 0.5f;
	
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_Lobby_Text::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Lobby_Text::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if(m_vTextArray.empty() == FALSE && m_fOffsetPosY >= 150)
		TextOutput(0.025f, fTimeDelta);	

	SimpleAnim(fTimeDelta);	

	if (m_bIsActive == FALSE && m_vTextArray.empty())
	{
		//Add_Text();
		m_bFinishText = TRUE;
	}
}

void CUI_Lobby_Text::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Lobby_Text::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
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
		_float2(225.f * 1.5f, 600.f * 1.5f),
		_vector{ 0.f,0.f,0.f,1.f },
		0.f,
		{ 0.f, 0.f },
		0.65f
	);

	return S_OK;
}

HRESULT CUI_Lobby_Text::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_UI_Lobby_TextBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CUI_Lobby_Text::Add_Text(queue<UI_TEXT> TextInfo)
{
	switch (m_vTextArray.size() != 0)
	{
		m_vTextArray.pop();
	}

	m_bFinishText = FALSE;
	m_vTextArray = TextInfo;
	//m_vTextArray.push({ TEXT("프리저! 네가 여기 나타난 순간부터 싸울 각오 했어! 이제 물러설 곳은 없어!"), CUI_Define::NPC_KRILLIN });
	//m_vTextArray.push({ TEXT("하하하! 네가 감히 나에게 덤비다니, 웃음을 주는군. 하지만 오래가진 못할 거야."), CUI_Define::NPC_FRIEZA });
	//m_vTextArray.push({ TEXT("내 힘을 무시하지 마라! 모두를 지키기 위해 난 싸운다!"), CUI_Define::NPC_KRILLIN });
	//m_vTextArray.push({ TEXT("이것이 네 최선인가? 내 손끝 하나로 너를 끝내주마."), CUI_Define::NPC_FRIEZA });
	//m_vTextArray.push({ TEXT("프리저, 네 상대는 나야! 크리링, 쉬고 있어. 이제 내가 나선다."), CUI_Define::NPC_GOKU});
	//m_vTextArray.push({ TEXT("오공... 또 네가 끼어드는군. 이번엔 널 완전히 없애주겠다!"), CUI_Define::NPC_FRIEZA });
}

void CUI_Lobby_Text::TextOutput(_float fOutputSpeed,_float fTimeDelta)
{
	m_fFontDration += fTimeDelta;

	_bool bFontFinish = (m_vTextArray.front().strText.length() == m_iTextIndex);

	if (m_fFontDration >= fOutputSpeed && bFontFinish == FALSE)
	{
		m_strOutputText += m_vTextArray.front().strText.at(m_iTextIndex);
		m_iTextIndex++;
		m_fFontDration = 0.f;
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextCharaIcon"))->SetActive(TRUE);
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextCursor"))->SetActive(TRUE);
		m_pUI_Manager->m_eNpcID = m_vTextArray.front().eNpcID;

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

void CUI_Lobby_Text::SimpleAnim(_float fTimeDelta)
{
	m_vTextArray.empty() ?  m_fOffsetPosY -= fTimeDelta * 600.f : m_fOffsetPosY += fTimeDelta * 600.f;

	if (m_fOffsetPosY >= 150.f)
		m_fOffsetPosY = 150.f;

	else if (m_fOffsetPosY <= 0.f)
	{
		m_fOffsetPosY = 0.f;
		m_bIsActive = FALSE;
	}
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY - m_fOffsetPosY, 0.f);
}

CUI_Lobby_Text* CUI_Lobby_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Lobby_Text* pInstatnce = new CUI_Lobby_Text(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Lobby_Text"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Lobby_Text::Clone(void* pArg)
{
	CUI_Lobby_Text* pInstatnce = new CUI_Lobby_Text(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Lobby_Text"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Lobby_Text::Free()
{
	__super::Free();
}
