#include "stdafx.h"

#include "UI_Opt_Sound_Volume_Gauge.h"
#include "RenderInstance.h"

CUI_Opt_Sound_Volume_Gauge::CUI_Opt_Sound_Volume_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Opt_Sound{ pDevice ,pContext }
{
}

CUI_Opt_Sound_Volume_Gauge::CUI_Opt_Sound_Volume_Gauge(const CUI_Opt_Sound_Volume_Gauge& Prototype)
	:CUI_Opt_Sound{ Prototype }
{
}

HRESULT CUI_Opt_Sound_Volume_Gauge::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Opt_Sound_Volume_Gauge::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fPosX = 770.f, m_fPosY = 320.f;
	m_fSizeX = 150.f, m_fSizeY = 20.f;

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);
	m_iNumUI = pDesc->iNumUI;

	PostionUpdate();

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);

	return S_OK;
}

void CUI_Opt_Sound_Volume_Gauge::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_Opt_Sound_Volume_Gauge::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if(m_bSoundEnable == FALSE)
		m_fSoundDelay += fTimeDelta * (1.f + m_fSoundWeight);

	if (m_fSoundDelay >= 0.15f)
	{
		m_bSoundEnable = TRUE;
		m_fSoundDelay = 0.f;
	}
		
	if (m_bKeyInput)
		PostionUpdate();

	SetVolume(fTimeDelta);
}

void CUI_Opt_Sound_Volume_Gauge::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_Opt_Sound_Volume_Gauge::Render(_float fTimeDelta)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pShaderCom->Begin(22)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	NumberFont();


	return S_OK;
}

HRESULT CUI_Opt_Sound_Volume_Gauge::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptVolumeGauge"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Opt_Sound_Volume_Gauge::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	for (int i = 0; i < MENU_END; ++i)
	{
		if (i == m_iNumUI)
		{
			_float fVolumeRadio = (_float)(m_fVolumeValue[i] / 100.f);

				if (FAILED(m_pShaderCom->Bind_RawValue("g_Radio", &fVolumeRadio, sizeof(_float))))
					return E_FAIL;
		}
	}
	return S_OK;
}

void CUI_Opt_Sound_Volume_Gauge::PostionUpdate()
{
	switch (m_iNumUI)
	{
	case BGM:
		m_fPosY = 320.f;
		break;

	case VOICE:
		m_fPosY = 360.f;
		break;

	case SFX:
		m_fPosY = 400.f;
		break;
	}

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.f);
}

void CUI_Opt_Sound_Volume_Gauge::SetVolume(_float fTimeDelta)
{
	switch (m_eMenuValue)
	{
	case BGM:
		KeyInput(BGM , fTimeDelta);
		break;

	case SFX:
		KeyInput(SFX , fTimeDelta);
		break;

	case VOICE:
		KeyInput(VOICE , fTimeDelta);
		break;
	}

	for (int i = 0; i < MENU_END; ++i)
	{
		if (m_fVolumeValue[i] <= 0)
		{
			m_fVolumeValue[i] = 0;
		}

		if(m_fVolumeValue[i] >= 100)
			m_fVolumeValue[i] = 100;

		m_pGameInstance->Set_Category_Volume(static_cast<CSound_Manager::SOUND_CATEGORY>(i) , m_fVolumeValue[i] * 0.01f);
	}

}

void CUI_Opt_Sound_Volume_Gauge::KeyInput(SOUND_MENU eSound , _float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
	{
		//if (m_bSoundEnable)
		//{
		//	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::UI_MENU_CURSOR, false, 1.f);
		//	m_bSoundEnable = FALSE;
		//}
		m_fVolumeValue[eSound]--;
		m_fSoundWeight += fTimeDelta;
	}
	else if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		//if (m_bSoundEnable)
		//{
		//	m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::UI_MENU_CURSOR, false, 1.f);
		//	m_bSoundEnable = FALSE;
		//}

		m_fVolumeValue[eSound]++;
		m_fSoundWeight += fTimeDelta;
	}
	else
		m_fSoundWeight = 0.f;

	if (m_fSoundWeight >= 1.5f)
		m_fSoundWeight = 1.5f;
}

void CUI_Opt_Sound_Volume_Gauge::NumberFont()
{
	_wstring strVolume =  to_wstring(0);
	_vector vColor = { 0.043f, 0.952f, 0.945f ,1.f };

	switch (m_iNumUI)
	{
	case BGM:
		strVolume = to_wstring(m_fVolumeValue[BGM]);

		if(m_fVolumeValue[BGM] == 50)
			vColor = { 0.996f, 0.729f, 0.f ,1.f };
		break;

	case SFX:
		strVolume = to_wstring(m_fVolumeValue[SFX]);

		if (m_fVolumeValue[SFX] == 50)
			vColor = { 0.996f, 0.729f, 0.f ,1.f };
		break;

	case VOICE:
		strVolume = to_wstring(m_fVolumeValue[VOICE]);

		if (m_fVolumeValue[VOICE] == 50)
			vColor = { 0.996f, 0.729f, 0.f ,1.f };
		break;

	}

	m_pGameInstance->Draw_Font(TEXT("Font_Nexon"),
		strVolume.c_str(),
		_float2((m_fPosX + 73) * 1.5f, (m_fPosY - 10) * 1.5f),
		vColor,
		0.f,
		{ 0.f, 0.f },
		0.7f
	);
}


CUI_Opt_Sound_Volume_Gauge* CUI_Opt_Sound_Volume_Gauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Opt_Sound_Volume_Gauge* pInstatnce = new CUI_Opt_Sound_Volume_Gauge(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Opt_Sound_Volume_Gauge"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_Opt_Sound_Volume_Gauge::Clone(void* pArg)
{
	CUI_Opt_Sound_Volume_Gauge* pInstatnce = new CUI_Opt_Sound_Volume_Gauge(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_Opt_Sound_Volume_Gauge"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_Opt_Sound_Volume_Gauge::Free()
{
	__super::Free();
}
