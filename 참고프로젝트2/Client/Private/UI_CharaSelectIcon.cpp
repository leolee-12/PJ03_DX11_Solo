#include "stdafx.h"

#include "UI_CharaSelectIcon.h"
#include "UI_SelectArrow.h"
#include "UI_ChoiceIcon.h"
#include "RenderInstance.h"
#include "UI_CharaSelectImage.h"
#include "UI_SelectLine.h"
#include "CharaSelect_Model.h"

#include "BattleInterface.h"

CUI_CharaSelectIcon::CUI_CharaSelectIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice ,pContext }
{
}

CUI_CharaSelectIcon::CUI_CharaSelectIcon(const CUI_CharaSelectIcon& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_CharaSelectIcon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CharaSelectIcon::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	UI_DESC* Desc = static_cast<UI_DESC*>(pArg);

	m_iTexIndex = Desc->iNumUI;
	m_fPosX = 150.f + (m_iTexIndex * 320), m_fPosY = 620.f;
	m_fSizeX = 150.f, m_fSizeY = 150.f;

	m_pArrowTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_CHARACTER, TEXT("Layer_MarkArrow"), TEXT("Com_Transform")));
	Safe_AddRef(m_pArrowTransform);

	if (m_iTexIndex == 0)
	{
		CreateSelectLine();
		CreateCharaImage();
	}
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.8f);

	return S_OK;
}

void CUI_CharaSelectIcon::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CUI_CharaSelectIcon::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	_vector vArrowPos = m_pArrowTransform->Get_State(CTransform::STATE_POSITION);

	_float fPosX = XMVectorGetX(vArrowPos) + g_iWinSizeX * 0.5f;
	_float fPosY = g_iWinSizeY * 0.5f - XMVectorGetY(vArrowPos);
	
	SelectIcon(fPosX, fPosY);
}

void CUI_CharaSelectIcon::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRenderInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CUI_CharaSelectIcon::Render(_float fTimeDelta)
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CharaSelectIcon::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharaSelect_Icon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CUI_CharaSelectIcon::SelectIcon(_float fPosX, _float fPosY)
{
	if (m_pGameInstance->Get_Layer(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")).empty())
	{
		m_fPosY = 620.f;
		__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.8f);
		return;
	}

	switch (m_iTexIndex)
	{
		case CUI_Define::GOKU:
			ClickRange(fPosX, fPosY) ? m_fPosY = 600.f, ArrowToPlayerID(CUI_Define::GOKU), InputEvent(DIK_RETURN, CUI_Define::GOKU) : m_fPosY = 620.f;
			break;

		case CUI_Define::ANDROID21:
			ClickRange(fPosX, fPosY) ? m_fPosY = 600.f , ArrowToPlayerID(CUI_Define::ANDROID21), InputEvent(DIK_RETURN, CUI_Define::ANDROID21) : m_fPosY = 620.f;
			break;

		case CUI_Define::FRIEZA:
			ClickRange(fPosX, fPosY) ? m_fPosY = 600.f, ArrowToPlayerID(CUI_Define::FRIEZA), InputEvent(DIK_RETURN, CUI_Define::FRIEZA) : m_fPosY = 620.f;
			break;

		case CUI_Define::HIT:
			ClickRange(fPosX, fPosY) ? m_fPosY = 600.f, ArrowToPlayerID(CUI_Define::HIT),InputEvent(DIK_RETURN, CUI_Define::HIT) : m_fPosY = 620.f;
			break;

		default:
			break;
	}

	if(ClickRange(fPosX, fPosY) == FALSE)
		m_bOnCursor = FALSE;
		
	__super::Set_UI_Setting(m_fSizeX, m_fSizeY, m_fPosX, m_fPosY, 0.8f);
}

void CUI_CharaSelectIcon::InputEvent(_uint iKey, CUI_Define::PLAYER_ID ePlayerID)
{
	if (m_pGameInstance->Key_Down(iKey))
	{
		if (OverlapCheck(ePlayerID) == FALSE)
			return;


		CreateChoiceMark(ePlayerID);
		CharacterCreateDesc(ePlayerID);
		CreateSelectModel(ePlayerID);

		dynamic_cast<CUI_SelectArrow*>(m_pGameInstance->Get_GameObject(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")))->SelectChoice();
		CreateSelectLine();
		CreateCharaImage();
	}
}

void CUI_CharaSelectIcon::CreateChoiceMark(CUI_Define::PLAYER_ID ePlayerID)
{
	_vector vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vOriginPos = XMVectorSetX(vOriginPos, (XMVectorGetX(vOriginPos) + g_iWinSizeX * 0.5f )  /  m_vOffSetWinSize.x);
	vOriginPos = XMVectorSetY(vOriginPos, (g_iWinSizeY * 0.5f - XMVectorGetY(vOriginPos)) / m_vOffSetWinSize.y);


	CUI_ChoiceIcon::UI_CHOICE_DESC ChoiceDesc = {};
	ChoiceDesc.vInitPosition = vOriginPos;
	ChoiceDesc.iTextrueIndex = dynamic_cast<CUI_SelectArrow*>(m_pGameInstance->Get_GameObject(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")))->Get_TextrueIndex();
	ChoiceDesc.iTeam = dynamic_cast<CUI_SelectArrow*>(m_pGameInstance->Get_GameObject(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")))->Get_NumChoice() / 2; //¿©±â¿¡¼­ 2´Â ÆÀ °¹¼ö
	//  0 1 2   3 4 5  2·Î ³ª´³À» ‹š 0ÀÌ¸é ¿ÞÂÊ 1ÀÌ¸é ¿À¸¥ÂÊ

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharaChoiceMark"), TEXT("Layer_BackGround") , &ChoiceDesc);
}

_bool CUI_CharaSelectIcon::OverlapCheck(CUI_Define::PLAYER_ID ePlayerID)
{
	_uint iTeam = dynamic_cast<CUI_SelectArrow*>(m_pGameInstance->Get_GameObject(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")))->Get_NumChoice() / 2;

	if (iTeam == 1 && m_bOverClear == FALSE)
	{
		m_ePlayerID.clear();
		m_bOverClear = TRUE;
	}


	m_ePlayerID.push_back(ePlayerID);

	for (int i = 0; i < CUI_Define::PAWN_END; ++i)
	{
		_uint NumOver = 0;
		for (int k = 0; k < m_ePlayerID.size(); k++)
		{
			if (m_ePlayerID[k] == i)
			{
				NumOver++;

				if (NumOver >= 2)
				{
					m_ePlayerID.pop_back();
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

void CUI_CharaSelectIcon::CharacterCreateDesc(CUI_Define::PLAYER_ID ePlayerID)
{
	_uint iNumChoice = dynamic_cast<CUI_SelectArrow*>(m_pGameInstance->Get_GameObject(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")))->Get_NumChoice();
	_ushort iTeam = { 0 };
	_uint SetIndex = { 0 };
	CUI_Define::PLAYER_SLOT ePlayerSlot = { CUI_Define::SLOT_END };

	switch (iNumChoice)
	{
	case 0:
		ePlayerSlot = CUI_Define::LPLAYER1;
		iTeam = 1;
		SetIndex = 0;
		break;

	case 1:
		ePlayerSlot = CUI_Define::LPLAYER2;
		iTeam = 1;
		SetIndex = 2;
		break;

	case 2:
		ePlayerSlot = CUI_Define::RPLAYER1;
		iTeam = 2;
		SetIndex = 1;
		break;

	case 3:
		ePlayerSlot = CUI_Define::RPLAYER2;
		iTeam = 2;
		SetIndex = 3;
		break;

	default:
		break;
	}


	wstring PrototypeTage = {};
	CUI_Define::PLAYER_ID ePlayerIDInfo= {};
	switch (ePlayerID)
	{
	case Client::CUI_Define::GOKU:
		PrototypeTage = TEXT("Prototype_GameObject_Play_Goku");
		ePlayerIDInfo = CUI_Define::GOKU;
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::NARRATION_CHARASELECT_GOKU, false, 1.f);

		break;
	case Client::CUI_Define::ANDROID21:
		PrototypeTage = TEXT("Prototype_GameObject_Play_21");
		ePlayerIDInfo = CUI_Define::ANDROID21;
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::NARRATION_CHARASELECT_21, false, 1.f);

		break;
	case Client::CUI_Define::FRIEZA:
		PrototypeTage = TEXT("Prototype_GameObject_Play_Frieza");
		ePlayerIDInfo = CUI_Define::FRIEZA;
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::NARRATION_CHARASELECT_FRN, false, 1.f);
		break;
	case Client::CUI_Define::HIT:
		PrototypeTage = TEXT("Prototype_GameObject_Play_Hit");
		ePlayerIDInfo = CUI_Define::HIT;
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::NARRATION_CHARASELECT_HIT, false, 1.f);

		break;

	default:
		break;
	}

	CBattleInterface_Manager::Get_Instance()->Set_CharaDesc(SetIndex,iTeam, ePlayerSlot, PrototypeTage, ePlayerIDInfo);
}

void CUI_CharaSelectIcon::CreateCharaImage()
{
	CUI_CharaSelectImage::UI_IMAGE_DESC ImageDesc = {};
	ImageDesc.iNumChoice = dynamic_cast<CUI_SelectArrow*>(m_pGameInstance->Get_GameObject(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")))->Get_NumChoice();

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharaSelectImage"), TEXT("Layer_BackGroundImage"), &ImageDesc);
}

void CUI_CharaSelectIcon::CreateSelectLine()
{
	CUI_SelectLine::UI_LINE_DESC LineDesc= {};
	LineDesc.iNumChoice = dynamic_cast<CUI_SelectArrow*>(m_pGameInstance->Get_GameObject(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")))->Get_NumChoice();
	
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharacterSelectLine"), TEXT("Layer_BackGround"), &LineDesc);
}

void CUI_CharaSelectIcon::CreateSelectModel(CUI_Define::PLAYER_ID ePlayerID)
{
	_uint iNumChoice = dynamic_cast<CUI_SelectArrow*>(m_pGameInstance->Get_GameObject(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")))->Get_NumChoice();

	CCharaSelect_Model::SELECT_MODEL SelectModelDesc = {};
	SelectModelDesc.ePlayerID = ePlayerID;
	SelectModelDesc.iNumModel = iNumChoice;
	
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHARACTER, TEXT("Prototype_GameObject_CharaSelect_Model"), TEXT("Layer_Character"),&SelectModelDesc);
}

void CUI_CharaSelectIcon::ArrowToPlayerID(CUI_Define::PLAYER_ID eID)
{
	if (m_bOnCursor == FALSE)
	{
		m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::UI_MENU_CURSOR, false, 1.f);
		m_bOnCursor = TRUE;
	}

	dynamic_cast<CUI_SelectArrow*>(m_pGameInstance->Get_GameObject(LEVEL_CHARACTER, TEXT("Layer_MarkArrow")))->SetPlayerID(eID);
}


CUI_CharaSelectIcon* CUI_CharaSelectIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CharaSelectIcon* pInstatnce = new CUI_CharaSelectIcon(pDevice, pContext);

	if (FAILED(pInstatnce->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_CharaSelectIcon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

CGameObject* CUI_CharaSelectIcon::Clone(void* pArg)
{
	CUI_CharaSelectIcon* pInstatnce = new CUI_CharaSelectIcon(*this);

	if (FAILED(pInstatnce->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloend : CUI_CharaSelectIcon"));
		Safe_Release(pInstatnce);
	}

	return pInstatnce;
}

void CUI_CharaSelectIcon::Free()
{
	Safe_Release(m_pArrowTransform);

	__super::Free();
}
