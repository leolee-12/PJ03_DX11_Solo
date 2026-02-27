#include "stdafx.h"

#include "UI_InputDir.h"
#include "UI_InputDirEffect.h"
#include "RenderInstance.h"

CUI_InputDir::CUI_InputDir(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Input{ pDevice ,pContext }
{
}

CUI_InputDir::CUI_InputDir(const CUI_InputDir& Prototype)
	:CUI_Input{ Prototype }
{
}

HRESULT CUI_InputDir::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_InputDir::Initialize(void* pArg)
{

	m_fPosX = 150.f;
	m_fPosY = 550.f;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 2.f;
	m_fSizeY = 2.f;

	m_vPos.x = m_fPosX;
	m_vPos.y = m_fPosY;

	ePrevDirInput = MOVEKEY_NEUTRAL;

	m_fOffsetScaled = 1.f;

	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.8f);
	 
	return S_OK;
}

void CUI_InputDir::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	m_pUI_Manager->m_fColorValue = m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI_EffectInput")).size() * 0.1f;
	
	if (m_pUI_Manager->m_fColorValue >= 1.f)
		m_pUI_Manager->m_fColorValue = 1.f;

}

void CUI_InputDir::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_bOnBtn)
		m_fOnTimer += fTimeDelta;

	DirectionInput eDirInput = MOVEKEY_NEUTRAL;
	if (m_eLRPos == LEFT)
		eDirInput = m_pUI_Manager->m_eDirInput;
	else if (m_eLRPos == RIGHT)
		eDirInput = m_pUI_Manager->m_eDirInput2;

	if (ePrevDirInput != eDirInput )
	{
		 ePrevDirInput = eDirInput;
		 m_bCheck = TRUE;
	}
	
	MovePos(eDirInput, m_fPosX, m_fPosY);
	LineEffectCreate();
	
	if (m_eLRPos == LEFT)
	{
		if (m_pUI_Manager->m_eBtnInput != ATTACK_NONE)
		{
			m_bOnBtn = TRUE;
			m_iTextureIndex = m_pUI_Manager->m_eBtnInput - 1;

			m_fOffsetScaled = 1.25f;
		}

		if (m_pUI_Manager->m_eBtnInput == ATTACK_GRAB)
			m_iTextureIndex = 1;
	}
	else if (m_eLRPos == RIGHT)
	{
		if (m_pUI_Manager->m_eBtnInput2 != ATTACK_NONE)
		{
			m_bOnBtn = TRUE;
			m_iTextureIndex = m_pUI_Manager->m_eBtnInput2 - 1;

			m_fOffsetScaled = 1.25f;
		}

		if (m_pUI_Manager->m_eBtnInput2 == ATTACK_GRAB)
			m_iTextureIndex = 1;
	}


	if (m_fOnTimer >= 0.1f)
	{
		m_fOnTimer = 0.f;
		m_bOnBtn = FALSE;
		m_iTextureIndex = 6;

		m_fOffsetScaled = 1.f;
	}



	
	__super::Set_UI_Setting(30.f * m_fOffsetScaled, 30.f * m_fOffsetScaled, m_fPosX, m_fPosY, 0.8f);
}

void CUI_InputDir::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_InputDir::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;



	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_InputDir::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ActionInput"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

_float CUI_InputDir::RotaionValue(_float2 vPrevPos, _float2 vCurrPos)
{
	_vector vAxis = { 0.f,1.f, 0.f , 0.f };
	_vector vDir = XMVector3Normalize((XMLoadFloat2(&vCurrPos) - XMLoadFloat2(&vPrevPos)));

	_float fAngle = XMVectorGetX(XMVector3Dot(vAxis, vDir));
	fAngle = acos(fAngle);
	

	if (XMVectorGetX(vDir) < 0.f)
	{
		fAngle = 3.14f * 2 - fAngle;
	}

	return XMConvertToDegrees(fAngle);
}

_float CUI_InputDir::ScaleValue(_float2 vPrevPos, _float2 vCurrPos)
{
	_float2 fPos = {};
	fPos.x = fabs(vCurrPos.x - vPrevPos.x);
	fPos.y = fabs(vCurrPos.y - vPrevPos.y);


	_float fDistance = sqrt(fPos.x * fPos.x + fPos.y * fPos.y);

	_float fScaled = 1;

	if(fDistance >100.f)
		fScaled = 2.2f ;
	else if(fDistance >65)
		fScaled = 1.5f;
	else 
		fScaled = 1.05f;

	return fScaled;
}

_vector CUI_InputDir::CreatePostion(_float2 vPrevPos, _float2 vCurrPos)
{
	_float fCreatePosX = vPrevPos.x + ((vCurrPos.x - vPrevPos.x) *0.5f );
	_float fCreatePosY = vPrevPos.y + ((vCurrPos.y - vPrevPos.y) * 0.5f);

	_vector vResultPos = { fCreatePosX  , fCreatePosY , 0, 1.f };
	return vResultPos;
}

void CUI_InputDir::MovePos(DirectionInput eInput, _float& fPosX, _float& fPosY)
{
	if (m_eLRPos == LEFT)
	{
		switch (eInput)
		{
		case DirectionInput::MOVEKEY_NEUTRAL:
			fPosX = 150.f;
			fPosY = 554.f;
			break;

		case DirectionInput::MOVEKEY_UP:
			fPosX = 150.f;
			fPosY = 508.f;
			break;

		case DirectionInput::MOVEKEY_DOWN:
			fPosX = 150.f;
			fPosY = 600.f;
			break;

		case DirectionInput::MOVEKEY_LEFT:
			fPosX = 100.f;
			fPosY = 554.f;
			break;

		case DirectionInput::MOVEKEY_RIGHT:
			fPosX = 200.f;
			fPosY = 554.f;
			break;

		case DirectionInput::MOVEKEY_UP_LEFT:
			fPosX = 110.f;
			fPosY = 514.f;
			break;

		case DirectionInput::MOVEKEY_UP_RIGHT:
			fPosX = 190.f;
			fPosY = 514.f;
			break;

		case DirectionInput::MOVEKEY_DOWN_LEFT:
			fPosX = 110.f;
			fPosY = 594.f;
			break;

		case DirectionInput::MOVEKEY_DOWN_RIGHT:
			fPosX = 190.f;
			fPosY = 594.f;
			break;
		}

		m_fPosX = fPosX;
		m_fPosY = fPosY;

	}
	else if (m_eLRPos == RIGHT)
	{
		switch (eInput)
		{
		case DirectionInput::MOVEKEY_NEUTRAL:
			fPosX = m_vPrevWinSize.x - 150.f ;
			fPosY = 554.f;
			break;

		case DirectionInput::MOVEKEY_UP:
			fPosX = m_vPrevWinSize.x - 150.f ;
			fPosY = 508.f;
			break;

		case DirectionInput::MOVEKEY_DOWN:
			fPosX = m_vPrevWinSize.x - 150.f ;
			fPosY = 600.f;
			break;

		case DirectionInput::MOVEKEY_LEFT:
			fPosX = m_vPrevWinSize.x - 100.f;
			fPosY = 554.f;
			break;

		case DirectionInput::MOVEKEY_RIGHT:
			fPosX = m_vPrevWinSize.x - 200.f;
			fPosY = 554.f;
			break;

		case DirectionInput::MOVEKEY_UP_LEFT:
			fPosX = m_vPrevWinSize.x - 110.f;
			fPosY = 514.f;
			break;

		case DirectionInput::MOVEKEY_UP_RIGHT:
			fPosX = m_vPrevWinSize.x - 190.f;
			fPosY = 514.f;
			break;

		case DirectionInput::MOVEKEY_DOWN_LEFT:
			fPosX = m_vPrevWinSize.x - 110.f;
			fPosY = 594.f;
			break;

		case DirectionInput::MOVEKEY_DOWN_RIGHT:
			fPosX = m_vPrevWinSize.x - 190.f;
			fPosY = 594.f;
			break;
		}

		m_fPosX = fPosX;
		m_fPosY = fPosY;
	}
	


}

void CUI_InputDir::LineEffectCreate()
{
	if (m_bCheck)
	{
		CUI_InputDirEffect::UI_DIREFFECT Desc = {};
		_float2 vMovePos = { m_fPosX , m_fPosY };

		Desc.vCreatePos = CreatePostion(m_vPos, vMovePos);
		Desc.fAngle = RotaionValue(m_vPos, vMovePos);
		Desc.fScaled = ScaleValue(m_vPos, vMovePos);

		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_UI_DirInputEffect"), TEXT("Layer_UI_EffectInput"), &Desc);

		m_vPos.x = m_fPosX;
		m_vPos.y = m_fPosY;

		m_bCheck = FALSE;
	}
}

CUI_InputDir* CUI_InputDir::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_InputDir* pInstatnce = new CUI_InputDir(pDevice, pContext);
	
	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_InputDir"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_InputDir::Clone(void* pArg)
{
	CUI_InputDir* pInstatnce = new CUI_InputDir(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_InputDir"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_InputDir::Free()
{
	__super::Free();
}
