#include "Level_GamePlay.h"
#include "Camera_Free.h"
#include "Player_LGPE.h"
#include "Effect_Star.h"
#include "UIButton_Glow.h"
#include "UIButton_Layered.h"
#include "UIButton_Group.h"
#include "Menu.h"
#include "Game_API.h"
#include "Level_Loading.h"
#include "Game_LevelEntry.h"
#include "Battle_Session.h"

#include "GameInstance.h"
#include "UISequence.h"

NS_BEGIN(Game_PKM)
static constexpr _uint CURRENT_LEVEL = ETOUI(LEVEL::GAMEPLAY);
NS_END

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(LAYER_EFFECT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	CCamera* pCamera = static_cast<CCamera*>(*(m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_CAMERA)->begin()));
	CPlayer_LGPE* pPlayer = static_cast<CPlayer_LGPE*>(*(m_pGameInstance->Get_ObjectList(ETOUI(LEVEL::GAMEPLAY), LAYER_PLAYER)->begin()));
	pCamera->Set_FollowTarget(pPlayer->Get_Transform());
	pCamera->Set_FollowOffset({ 0.f, 6.5f, -7.5f });
	m_pGameInstance->Set_MainCamera(pCamera);

	m_pGameInstance->Play_BGM(L"BGM/1-04. Pallet Town Theme.mp3", 0.5f);

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F2))
		m_pGameInstance->Toggle_CameraFollow();

	if (m_pGameInstance->Key_Down(DIK_F3))
		m_pGameInstance->Toggle_Debug();

	/* F4 — 메뉴 열기/닫기 토글. Open() 이 시퀀스 Play 도 같이 트리거. */
	if (m_pGameInstance->Key_Down(DIK_F4) && nullptr != m_pMenu)
	{
		if (m_pMenu->Is_Open())
			m_pMenu->Close();
		else
			m_pMenu->Open();
	}

	if (m_pGameInstance->Key_Down(DIK_P))
	{
		BATTLE_ENV tEnv = {};
		tEnv.eEnvironment = ENVIRONMENT_TYPE::GRASS;
		tEnv.eRule = BATTLE_RULE::WILD_SINGLE;
		tEnv.iBGResourceID = 0;
		tEnv.iZoneID = 0;

		LEVEL_ENTRY_DESC tEntryDesc = {};
		tEntryDesc.Clear();
		tEntryDesc.eNextLevelID = LEVEL::BATTLE;

		if (FAILED(tEntryDesc.Set_Payload(LEVEL_ENTRY_PAYLOAD::BATTLE_ENV, &tEnv, sizeof(BATTLE_ENV))))
			return;

		m_pGameInstance->Play_BGM(L"BGM/1-24. Battle! (Gym Leader).mp3", 0.5f);

		if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING),
			CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::BATTLE, &tEntryDesc))))
		{
			return;
		}
	}

	/* 등록된 모든 UI 컨트롤러에 Update 전파. 닫혀 있으면 베이스가 즉시 return. */
	UI_Update_All(fTimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(m_pGameInstance->Get_HWND(), TEXT("게임플레이레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	m_pGameInstance->Clear_Lights();

	LIGHT_DESC      LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(0.5f, -0.5f, 0.5f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	//LightDesc.eType = LIGHT::POINT;
	//LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	//LightDesc.vAmbient = _float4(0.05f, 0.f, 0.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 0.1f, 0.1f, 1.f);
	//LightDesc.vPosition = _float4(10.f, 5.f, 10.f, 1.f);
	//LightDesc.fRange = 15.f;
	//
	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;
	//
	//LightDesc.eType = LIGHT::POINT;
	//LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	//LightDesc.vAmbient = _float4(0.f, 0.05f, 0.f, 1.f);
	//LightDesc.vSpecular = _float4(0.1f, 1.f, 0.1f, 1.f);
	//LightDesc.vPosition = _float4(25.f, 5.f, 10.f, 1.f);
	//LightDesc.fRange = 15.f;
	//
	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	SHADOW_LIGHT_DESC ShadowDesc{};
	ShadowDesc.vEye = _float4(-10.f, 10.f, 0.f, 1.f);
	ShadowDesc.vAt = _float4(1.f, 0.f, 0.f, 1.f);
	ShadowDesc.fFovy = XMConvertToRadians(60.f);
	ShadowDesc.fNear = 0.1f;
	ShadowDesc.fFar = 200.f;

	if (FAILED(m_pGameInstance->Set_ShadowLight(ShadowDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(WNameID strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC CameraDesc = {};

	CameraDesc.vEye = _float3(0.f, 8.f, -7.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(35.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(180.f);
	CameraDesc.fMouseSensor = 0.03f;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), PROTO_OBJ_CAMERA_FREE,
		ETOUI(LEVEL::GAMEPLAY), strLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SKY, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_MAP_TOWN01, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_MAP_TOWN02, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_MAP_ROAD01, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SNOW,
	//	ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_PLAYER_LGPE, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(WNameID strLayerTag)
{
	//for (size_t i = 0; i < 20; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_MONSTER, ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(WNameID strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_EXPLOSION,
		ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(WNameID strLayerTag)
{
	CUISequence::UISEQUENCE_DESC tDesc{};
	tDesc.strPath = "../../DataFiles/UI/UI_Menu.uiseq";
	tDesc.iProtoLevel = ETOUI(LEVEL::STATIC);
	
	CUISequence* pSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tDesc));
	if (nullptr == pSeq)
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pSeq)))
	{
		Safe_Release(pSeq);
		return E_FAIL;
	}
	
	m_pRuntimeUI = pSeq;  // weak

	/* ===== 커서 시퀀스 — Hub 가 단일 인스턴스로 공유 ===== */
	{
		CUISequence::UISEQUENCE_DESC tCursorDesc{};
		tCursorDesc.strPath = "../../DataFiles/UI/UI_Cursor.uiseq";
		tCursorDesc.iProtoLevel = ETOUI(LEVEL::STATIC);

		CUISequence* pCursorSeq = static_cast<CUISequence*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::GAMEOBJECT, ETOUI(LEVEL::STATIC), PROTO_UI_SEQUENCE, &tCursorDesc));
		if (nullptr == pCursorSeq)
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_GameObject_Ex(CURRENT_LEVEL, strLayerTag, pCursorSeq)))
		{
			Safe_Release(pCursorSeq);
			return E_FAIL;
		}

		UI_Set_Cursor_Sequence(pCursorSeq);  // Hub 에 weak 주입
		m_pCursorSeq = pCursorSeq;           // 레벨도 weak (Add_GameObject_Ex 가 owner)
	}

	/* ===== 메뉴 컨트롤러 테스트 등록 ===== */
	CMenu* pMenu = CMenu::Create();
	if (nullptr == pMenu)
		return E_FAIL;

	if (FAILED(pMenu->Initialize(pSeq)))
	{
		Safe_Release(pMenu);
		return E_FAIL;
	}

	/* 활성화 콜백 — 어떤 항목이 선택됐는지 OutputDebugString 으로 확인 */
	pMenu->Set_OnActivate([](_int iIndex)
		{
			static constexpr const _char* s_Names[] =
			{
					"PARTNER", "DEX", "BAG", "ENTRY", "LINK", "REPORT"
			};
			if (iIndex >= 0 && iIndex < static_cast<_int>(CMenu::MENU_ENTRY::END))
			{
				OutputDebugStringA("[Menu] Activated: ");
				OutputDebugStringA(s_Names[iIndex]);
				OutputDebugStringA("\n");
			}
			else
			{
				OutputDebugStringA("[Menu] Activated: index out of range\n");
			}
		});

	/* 취소 콜백 — 베이스가 콜백 후 Close() 자동 호출 */
	pMenu->Set_OnCancel([]()
		{
			OutputDebugStringA("[Menu] Cancelled\n");
		});

	/* Hub 등록 — 내부 AddRef. 이후 local 레퍼런스 해제. */
	if (FAILED(UI_Register(pMenu)))
	{
		Safe_Release(pMenu);
		return E_FAIL;
	}

	m_pMenu = pMenu;        // weak — Hub 가 owner
	Safe_Release(pMenu);    // local ref-- (Hub 가 ref 보유 중이라 안전)

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	/* Hub 의 cursor weak 를 UI_Close_All 보다 먼저 끊어 안전성 확보.
	   (Hub::Update_Cursor 가 다음 프레임에 호출되더라도 m_pCursor == nullptr 분기로 빠짐.) */
	UI_Set_Cursor_Sequence(nullptr);
	UI_Close_All();

	m_pCursorSeq = nullptr;
	m_pMenu = nullptr;
	m_pRuntimeUI = nullptr;

	__super::Free();
}
