#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Effect_Manager.h"
#include "IMGUI_Shader_Tab.h"

//명승호
#include "Main_Camera.h"
#include "Virtual_Camera.h"
#include "Line_Draw.h"
//


#include "Model_Preview.h"
#include "Shadow_Camera.h"
#include "Shader_Texture.h"
#include "Effect_NoneLight.h"
#include "Effect_Blend.h"
#include "Effect_ZNone.h"
#include "Effect_Overlap.h"
#include "Effect_Layer.h"
#include "VolcanoEF.h"
#include "SpaceEF.h"
#include "SpaceSky.h"
#include "SpaceSun.h"
#include "SpaceRock.h"
#include "SpaceMoon.h"
#include "SpaceStone.h"
#include "SpaceCliff.h"
#include "SpaceEarth.h"
#include "SpaceStage.h"
#include "SpaceGround.h"
#include "SpaceHorizon.h"
#include "SpaceEarth_Light.h"
#include "SpaceMeteoBreak.h"
#include "FallingStar.h"
#include "SpaceRock.h"
#include "Volcano_Stage.h"
#include "Volcano_Cliff_Back.h"
#include "Volcano_Cliff_Far.h"
#include "Volcano_Cliff01.h"
#include "Volcano_Cliff02.h"
#include "Volcano_Cliff03.h"
#include "Volcano_Cliff04.h"
#include "Volcano_Ground.h"
#include "Volcano_GroundRock.h"
#include "Volcano_Island01.h"
#include "Volcano_Island02.h"
#include "Volcano_Lava_Fall.h"
#include "Volcano_Lava_Ground.h"
#include "Volcano_Lava_Pool.h"
#include "Volcano_SkyCloud.h"
#include "Volcano_Smoke.h"
#include "Volcano_Mountain.h"
#include "Volcano_Destructive.h"
#include "Frieza_Metal.h"
//LOGO
#include "UI_Logo_BG.h"
#include "UI_Logo_Mark.h"

//UI 헤더
#include "UI_Cursor.h"
#include "UI_HpPanel.h"
#include "UI_HpGauge.h"
#include "UI_SubHpGauge.h"
#include "UI_HpEffect.h"
#include "UI_Chara_Icon_Panel.h"
#include "UI_Sub_Chara_Icon_Panel.h"
#include "UI_Chara_Icon.h"
#include "UI_AttBuf.h"
#include "UI_AttBufEffect.h"
#include "UI_AttBufThunderEffect.h"
#include "UI_AttBufNone.h"
#include "UI_AttBufMark.h"
#include "UI_SkillGauge.h"
#include "UI_SkillGaugeBar.h"
#include "UI_SkillNumber.h"
#include "UI_SkillEffect.h"
#include "UI_Timer.h"
#include "UI_TimerPanel.h"
#include "UI_ComboNumber.h"
#include "UI_ComboFont.h"
#include "UI_ComboEffect.h"
#include "UI_GameStartCircle.h"
#include "UI_StartEmblem.h"
#include "UI_ReadyFont.h"
#include "UI_FightFont.h"
#include "UI_KOFont.h"
#include "UI_KOFontEffect.h"
#include "UI_Chara_SubIcon.h"
#include "UI_InputDir.h"
#include "UI_Input_DirPanel.h"
#include "UI_Input_Action.h"
#include "UI_InputDirIcon.h"
#include "UI_Input_ActionIcon.h"
#include "UI_InputDirEffect.h"
#include "UI_Input_IconPanel.h"
#include "UI_FontName.h"
#include "UI_KOPanel.h"
#include "UI_KOParticle.h"
#include "UI_Loading_BG.h"
#include "UI_LoadingMark.h"
#include "UI_Loading_Font.h"
#include "UI_AttBufIconEff.h"
#include "UI_SubHpPanel.h"
#include "UI_Opt_Sound_Panel.h"
#include "UI_Opt_Sound_Arrow.h"
#include "UI_Opt_Sound_Volume_Panel.h"
#include "UI_Opt_Sound_Volume_Gauge.h"
#include "UI_Opt_Sound_Eff.h"
#include "UI_Opt_Sound_Font.h"
#include "UI_Opt_Sound_Title.h"
#include "UI_Chara_Select_BG.h"
#include "UI_CharaSelectIcon.h"
#include "UI_SelectArrow.h"
#include "UI_CharaSelectImage.h"
#include "UI_ChoiceIcon.h"
#include "UI_SelectLine.h"
#include "UI_CharaSelectFont.h"
#include "UI_CharaSelectFude.h"
#include "UI_CharaSelectMark.h"
#include "CharaSelect_Model.h"
#include "UI_CharaSelectLight.h"
#include "UI_CharaSelectCircle.h"
#include "UI_VS_BG.h"
#include "UI_VS_Mark.h"
#include "UI_VS_CharaPanel.h"
#include "UI_VS_StaticLight.h"
#include "UI_VS_DynamicLight.h"
#include "UI_VS_Bar.h"
#include "UI_VS_BoltEff.h"
#include "UI_VS_MarkEff.h"
#include "UI_VS_Ball.h"
#include "UI_VS_TeamPanel.h"
#include "UI_VS_Name.h"
#include "UI_VS_NameOutLine.h"
#include "UI_Loading_EnergyEff.h"
#include "UI_Win_Font.h"
#include "UI_Win_Particle.h"

#include "Character.h"
#include "Play_Goku.h"
#include "Play_21.h"
#include "Play_Hit.h"
#include "Play_Frieza.h"
#include "AttackObject.h"
#include "AttackObject_Chase.h"
#include "AttackObject_Grab.h"
#include "AttackObject_CommandGrab.h"
#include "AttackObject_Ranged.h"
#include "AttackObject_Energy.h"
#include "AttackObject_Reflect.h"
#include "ParryingRangedObject.h"
#include "Opening_Kririn.h"

#include "BoneEffectObject.h"
#include "QTE_Same_Grab_UI_Icon.h"
#include "QTE_Same_Grab.h"
#include "QTE_Same_Grab_UI_Gauge.h"
#include "QTE_Hit.h"
#include "QTE_Hit_UI_Icon.h"
#include "QTE_Hit_Situation.h"
#include "QTE_Hit_UI_MovingRing_Icon.h"
#include "QTE_Hit_UI_Result.h"
#include "QTE_Continuous_Attack.h"
#include "QTE_Continuous_Attack_Space.h"
#include "QTE_Continuous_Attack_Gauge.h"
#include "QTE_Hit_UI_Particle.h"
#include "QTE_Continuous_Attack_Effect.h"
#include "QTE_Same_Grab_UI_Particle.h"
#include "QTE_Continuous_Attack_Particle.h"
#include "QTE_Continuous_Attack_Space_Particle.h"
#include "QTE_1P_Same_Grab.h"
#include "Particle_Spread.h"
#include "Particle_Focus.h"
#include "Particle_Common_Hit.h"
#include "Particle_Frieza_1_Ultimate_Hit.h"
#include "Particle_Frieza_3_Ultimate_Hit.h"
#include "Particle_21_3_Ultimate.h"
#include "SubTitle.h"

//Lobby
#include "Lobby_Center_Map.h"
#include "Main_Camera_Lobby.h"
#include "Lobby_Battle_Building.h"
#include "Local_Battle_Building.h"
#include "Lobby_Story_Mode_Building.h"
#include "Lobby_Arcade_Building.h"
#include "Lobby_Parasol.h"
#include "Lobby_Goku.h"
#include "Lobby_Frieza.h"
#include "Lobby_Krillin.h"
#include "Lobby_Sky.h"
#include "Lobby_Sky_Of_Sea.h"
#include "Lobby_Goku_RunEff.h"
#include "Lobby_DisplayBoard.h"
#include "Lobby_Cloud.h"
#include "Lobby_Wring.h"
#include "Lobby_Car.h"
#include "Lobby_Flag.h"
#include "Lobby_Ranking.h"
#include "Lobby_Party.h"
#include "Lobby_Staff.h"
#include "UI_Lobby_Text.h"
#include "UI_Lobby_TextCharaIcon.h"
#include "UI_Lobby_TextCursor.h"
#include "UI_Lobby_Key_Enter.h"
#include "UI_Win_Circle.h"
#include "UI_Win_Team.h"
#include "CharaSelect_Camera.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	/* 어떤 레벨에 대한 준비를 해야하는지 */
	m_eLevelID = eNextLevelID;

	m_pGameInstance->Set_LoadingLevel_Index(m_eLevelID);

	if (FAILED(Loading()))
		return E_FAIL;

	return S_OK;
}

/* 추가적으로 생성한 스레드가 호출한 함수. */
HRESULT CLoader::Loading()
{
	HRESULT		hr{};

	m_isFinished = false;

	switch (m_eLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo();
		break;

	case LEVEL_LOBBY:
		hr = Loading_For_Lobby();
		break;

	case LEVEL_CHARACTER:
		hr = Loading_For_CharaSelect();
		break;

	case LEVEL_VS:
		hr = Loading_For_VS();
		break;

	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

atomic_bool CLoader::isFinished()
{
	if (m_isFinished)
		return true;

	for (auto it = m_taskInfos.begin(); it != m_taskInfos.end(); )
	{
		if (it->future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			try
			{
				HRESULT result = it->future.get();
				if (FAILED(result))
				{
					// 실패 처리 (예: 로그 기록, 에러 플래그 설정 등)
					m_isFinished = true;
					return false;
				}
				else
				{
					// 작업이 성공적으로 완료됨. 큐에 enum 값을 추가
					CUI_Manager::Get_Instance()->AddToQueue(it->taskType);
				}
			}
			catch (const std::exception& e)
			{
				// 예외 처리 (예: 로그 기록, 에러 플래그 설정 등)
				m_isFinished = true;
				return false;
			}
			// 완료된 작업을 리스트에서 제거
			it = m_taskInfos.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (m_taskInfos.empty())
	{
		m_isFinished = true;
		return true;
	}

	return false;
}

HRESULT CLoader::Loading_For_Loading()
{
	//선
/* For.Prototype_Component_Texture_UI_LoadingBackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CmnBG/tex/E3_Title_BG01.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_LoadingBackGround_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingBackGround_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CharaSelect_S3/tex/stage/stage_bg_0.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_GameStartCircle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingMark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Middle/GameStart/GameStart%d.png"), 8))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_UI_LoadingFont */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LoadingFont"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/DB_load_00.png")))))
		return E_FAIL;


	/* For.Prototype_GameObject_UI_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading"),
		CUI_Loading_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_LoadingMark */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LoadingMark"),
		CUI_LoadingMark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_LoadingFont */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LoadingFont"),
		CUI_Loading_Font::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	Load_UI_Resources_Logo();

	return S_OK;
}

HRESULT CLoader::Loading_For_Lobby()
{
	//텍스처
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_UI_Lobby_TextBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Text/Z_League_WindowTxtBG.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_UI_Lobby_TextCharaIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/LIVEChar%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_UI_Lobby_TextCursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CmnMenu/tex/window_arrow_key_02.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_UI_Lobby_Key_Enter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Lobby/Key_Enter.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_UI_Lobby_Goku_RunEff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Lobby/Lobby_Goku_Dust%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_DisplayBoard"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Lobby/lob_SignLine.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_DisplayFont"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Lobby/lob_SignLineFont%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_UI_Lobby_StaffTextBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Lobby/TextBox.png")))))
		return E_FAIL;

	_matrix			PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	//모델
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Center"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Center.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Battle_Building"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Battle_Building.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Local_Battle_Building"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Local_Battle_Building.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Story_Mode_Building"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Story_Mode_Building.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Arcade_Mode_Building"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Arcade_Mode_Building.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Parasol"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Parasol.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Sky"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Sky.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Cloud0"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Cloud_0.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Cloud1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Cloud_1.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Cloud2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Cloud_2.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Cloud3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Cloud_3.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Cloud4"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Cloud_4.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Sky_Sea_Of_Cloud"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Sky_Sea_Of_Cloud.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationX(XMConvertToRadians(270.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_DIsplayBoard_In"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Circle_Eff.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_DIsplayBoard_Out"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Circle_OutEff.bin", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Goku"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Goku.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Frieza"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Frieza.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Krillin"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Lobby_Krillin.bin", PreTransformMatrix))))
		return E_FAIL;


	//New Mdoel Data
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Arcade"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_arcade.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Audience"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_audience.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Car"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_car.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_CenterMap"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_center.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_DLeague"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_DLeague.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_FlagB"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_flagB.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_FlagR"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_flagR.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_FlagY"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_flagY.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_FloatingIslandA"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_floatingIslandA.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_FloatingIslandB"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_floatingIslandB.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_FloatingIslandC"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_floatingIslandC.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Fountain"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_fountain.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Globe"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_globe.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_KaiouS"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_KaiouS.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Lobbymatch"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_lobbymatch.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Local"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_local.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_PalmA"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_palmA.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_PalmB"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_palmB.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_PalmC"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_palmC.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Parasol"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_parasol.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Party"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_party.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_RaidBoss"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_RaidBoss.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Ranking"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_ranking.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Replay"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_replay.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Screen"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_screen.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Screen02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_screen02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_SeaOfClouds"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_SeaOfClouds.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Sign"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_sign.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_SignRingL1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_SignRingL1.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_SignRingU"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_SignRingU.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_SignRingUcut"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_SignRingUcut.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_SignRingV"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_SignRingV.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_SkyMap"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_sky.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Step"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_step.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Story"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_story.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Town"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_town.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Training"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_training.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Wring"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Lobby/Mesh/lob_Wring.bin", PreTransformMatrix))))
		return E_FAIL;

	//NPC 모델 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_NPC_Staff"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Char/Lobby_NPC/NPC_Staff.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_NPC_BUN"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Char/Lobby_NPC/NPC_BUN.bin", PreTransformMatrix))))
		return E_FAIL;

	//New Model Texture

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_TreeReaf02_sh"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/cg_TreeReaf02_sh.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_audience"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_audience%d.png"), 23))))
		return E_FAIL;

	//Wring
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_lawn_Edge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_lawn_Edge.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_seat_outside"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_seat_outside.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_LIlandSide"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_LIlandSide.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_banner"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_banner.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_road"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_road.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_lawn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_lawn.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_seat01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_seat01.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_RingSide03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_RingSide03.png")))))
		return E_FAIL;

	//Car
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_Car"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_Car.png")))))
		return E_FAIL;

	//Flag
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_flag_rod"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_flag_rod.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_flagY_anim"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_flagB_anim.png")))))
		return E_FAIL;

	//Rank
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_singboard"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_singboard.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_ranking"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_ranking.png")))))
		return E_FAIL;

	//Party
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PTree"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_PTree.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PIlandSide"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_PIlandSide.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_KameH"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_KameH.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_PGround.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PWaterFall"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_PWaterFall.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PSea"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_PSea.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PWaterAlpha"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_PWaterAlpha.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PWaterLight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_PWaterLight.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PWave01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_PWave01.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PWave02_psd"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_PWave02_psd.png")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Texture_Lobby_lob_PWaterSurface"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Lobby/Texture/lob_PWaterSurface.png")))))
		return E_FAIL;


	//컴포넌트 

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_Shader_Particle_VtxPoint"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_VtxPoint.hlsl"), VTXPARTICLE_POINT::Elements, VTXPARTICLE_POINT::iNumElements))))
		return E_FAIL;


	CVIBuffer_Instancing::VIBUFFER_INSTANCE_DESC	ParticleDesc{};
	ParticleDesc.iNumInstance = 5;
	ParticleDesc.vRange = _float3(1.f, 1.f, 0.f);
	ParticleDesc.vCenter = _float3(0.0f, 0.f, 0.f);
	ParticleDesc.vPivot = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vSpeed = _float2(1.5f, 2.f);
	ParticleDesc.vScale = _float2(0.5f, 1.f);
	ParticleDesc.vLifeTime = _float2(0.25f, 1.f);
	ParticleDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOBBY, TEXT("Prototype_Component_VIBuffer_Particle_Lobby_Goku_RunEff"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	//게임오브젝트

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Center_Map"),
		CLobby_Center_Map::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Battle_Building"),
		CLobby_Battle_Building::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Local_Battle_Building"),
		CLocal_Battle_Building::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Story_Mode_Building"),
		CLobby_Story_Mode_Building::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Arcade_Building"),
		CLobby_Arcade_Building::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Parasol"),
		CLobby_Parasol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Goku"),
		CLobby_Goku::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Frieza"),
		CLobby_Frieza::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Krillin"),
		CLobby_Krillin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Sky"),
		CLobby_Sky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Sky_Of_Sea"),
		CLobby_Sky_Of_Sea::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Main_Camera_Lobby"),
		CMain_Camera_Lobby::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_TextBox"),
		CUI_Lobby_Text::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_TextCharaIcon"),
		CUI_Lobby_TextCharaIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_TextCursor"),
		CUI_Lobby_TextCursor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Key_Enter"),
		CUI_Lobby_Key_Enter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Goku_RunEff"),
		CLobby_Goku_RunEff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_DisplayBoard"),
		CLobby_DisplayBoard::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Cloud"),
		CLobby_Cloud::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Lobby_Wring"),
		CLobby_Wring::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Lobby_Car"),
		CLobby_Car::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Lobby_Flag"),
		CLobby_Flag::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Lobby_Ranking"),
		CLobby_Ranking::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Lobby_Party"),
		CLobby_Party::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lobby_Lobby_Staff"),
		CLobby_Staff::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading_For_VS()
{
	/* For.Prototype_Component_Texture_UI_VS_BG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/BackGround.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_BG_Color */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Color"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/vs_bg_00.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_BG_Line */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Line"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/vs_bg_07.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_Mark */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_Mark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/LOC/vs_object_%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_CharaPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_CharaPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/vs_object_02b.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_CharaPanelPlate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_CharaPanelPlate"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/vs_plate_00.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CharIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_CharaIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Chara_Image//Face_A/Chara_Icon/CharaIcon%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_StaticLight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_StaticLight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/vs_effect_00.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_DynamicLight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_DynamicLight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/vs_effect_01.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_BG_Bar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/vs_bg%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_BG_Circle0 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Circle0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/Circle0.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_BG_Circle1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Circle1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/Circle1.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_BG_Circle2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Circle2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/Circle2.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_BG_Circle3 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Circle3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/Circle3.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_BG_Bolt */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_Bolt"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/vs_Bolt%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_BG_DragonBall */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_BG_DragonBall"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/Ball/Ball%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_VS_TeamPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_VS_TeamPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/vsinfo/tex/VS_Panel.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_FontName */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VS, TEXT("Prototype_Component_Texture_UI_FontName"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Chara_Name%d.png"), 4))))
		return E_FAIL;

	/* Prototype_GameObject_VS_BG */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_BG"),
		CUI_VS_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_VS_Mark */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_Mark"),
		CUI_VS_Mark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_VS_CharaPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_CharaPanel"),
		CUI_VS_CharaPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_VS_StaticLight */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_StaticLight"),
		CUI_VS_StaticLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_VS_DynamicLight */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_DynamicLight"),
		CUI_VS_DynamicLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_VS_Bar*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_Bar"),
		CUI_VS_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_VS_Bolt */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_Bolt"),
		CUI_VS_BoltEff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_VS_MarkEff */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_MarkEff"),
		CUI_VS_MarkEff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_VS_Ball */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_Ball"),
		CUI_VS_Ball::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_VS_TeamPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_TeamPanel"),
		CUI_VS_TeamPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_VS_Name */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_Name"),
		CUI_VS_Name::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_VS_NameOutLine */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VS_NameOutLine"),
		CUI_VS_NameOutLine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_CharaSelect()
{
	/* For.Prototype_Component_Texture_CharaSelect_BG */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharaSelect_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CmnBG_Main_parts01.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CharaSelect_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharaSelect_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CharacterImage/CS_CIcon%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CharaSelect_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharaSelect_Image"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CharacterImage/CS_CharacterImage_%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CharacterSelectArrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharacterSelectArrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CS_PlayerCursor_%d.png"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CharacterChoiceMark */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharacterChoiceMark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CS_PlayerCursor_Choice_%d.png"), 6))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CharacterBGMask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharacterBGMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CS_CharacterBG_Mask%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CharacterSelectLine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_CharacterSelectLine"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CS_ChoiceBG_01.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CharacterSelectFont */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectFont"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CS_Title.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CharacterSelectFude */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectFude"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CmnBG_BigFude%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CharacterSelectLineFrame*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectLineFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CmnBG_LineFrame.png")))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_UI_CharacterSelectDustEffect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectDustEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/vs_bg_08.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CharacterSelectMark*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectMark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CmnBG_Bu.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CharacterSelectLight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectLight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/CmnBG_Eff_Flare0%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CharacterSelectCircle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_CharacterSelectCircle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Select_Char_And_Map/vs_effect_00.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_LoadingEnergyEff */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_UI_LoadingEnergyEff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Loading/CmnBG_Eff_Lens_5.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Character_Goku_Base */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_Goku_Base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/GKS_base.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Character_21_Base */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_21_Base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/TON_base.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Character_Frieza_Base */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_Frieza_Base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Frieza/FRN_base_1P.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Character_Hit_Base */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_Hit_Base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/HTN_base.png")))))
		return E_FAIL;

	_matrix			PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Model_CharaSelectMddel_Goku"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_Ready_Allmesh.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Model_CharaSelectMddel_21"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/TON_Ready_Allmesh.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Model_CharaSelectMddel_Frieza"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/FRN_Ready_Allmesh.bin", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(270.f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Model_CharaSelectMddel_Hit"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Hit_Select.bin", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Character_OutLine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_OutLine_Goku"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GKS_ilm.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Character_OutLine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_OutLine_21"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/TON_ilm.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Character_OutLine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_OutLine_Frieza"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Frieza/FRN_ilm.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Character_OutLine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CHARACTER, TEXT("Prototype_Component_Texture_Character_OutLine_Hit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/HTN_ilm.png"), 1))))
		return E_FAIL;

	/* Prototype_GameObject_CharaSelectBG */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharaSelectBG"),
		CUI_Chara_Select_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_CharaSelectIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharaSelectIcon"),
		CUI_CharaSelectIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_CharaSelectArrow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharaSelectArrow"),
		CUI_SelectArrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_CharaSelectImage */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharaSelectImage"),
		CUI_CharaSelectImage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_CharaChoiceMark */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharaChoiceMark"),
		CUI_ChoiceIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_CharacterSelectLine */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharacterSelectLine"),
		CUI_SelectLine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_CharacterSlectFont */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharacterSelectFont"),
		CUI_CharaSelectFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_CharacterSlectFude */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharacterSelectFude"),
		CUI_CharaSelectFude::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* Prototype_GameObject_CharacterSlectMark */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharacterSelectMark"),
		CUI_CharaSelectMark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_CharacterSelectLight */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharacterSelectLight"),
		CUI_CharaSelectLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_CharacterSelectCircle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharacterSelectCircle"),
		CUI_CharaSelectCircle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Loading_EnergyEff */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Loading_EnergyEff"),
		CUI_Loading_EnergyEff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_CharaSelect_Camera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_CharaSelect_Camera"),
		CCharaSelect_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_CharaSelect_Model */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharaSelect_Model"),
		CCharaSelect_Model::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	m_taskInfos.push_back({ m_pGameInstance->EnqueueTask([this]() { return Load_Texture_Resources_GamePlay_0(); }), CUI_Manager::THREAD_TEXTURE_0 });
	m_taskInfos.push_back({ m_pGameInstance->EnqueueTask([this]() { return Load_Texture_Resources_GamePlay_1(); }), CUI_Manager::THREAD_TEXTURE_1 });
	m_taskInfos.push_back({ m_pGameInstance->EnqueueTask([this]() { return Load_Texture_Resources_GamePlay_2(); }), CUI_Manager::THREAD_TEXTURE_2 });
	m_taskInfos.push_back({ m_pGameInstance->EnqueueTask([this]() { return Load_Model_Resources_GamePlay_0(); }), CUI_Manager::THREAD_MODEL_0 });
	m_taskInfos.push_back({ m_pGameInstance->EnqueueTask([this]() { return Load_Model_Resources_GamePlay_1(); }), CUI_Manager::THREAD_MODEL_1 });
	m_taskInfos.push_back({ m_pGameInstance->EnqueueTask([this]() { return Load_Prototype_Object_GamePlay(); }), CUI_Manager::THREAD_OBJECT_0 });
	m_taskInfos.push_back({ m_pGameInstance->EnqueueTask([this]() { return Load_Prototype_Component_GamePlay(); }), CUI_Manager::THREAD_COMPONENT_0 });
	// 즉시 반환하여 메인 스레드가 계속 실행되도록 함
	return S_OK;
}

HRESULT CLoader::Load_UI_Resources_Logo()
{
	{
		std::lock_guard<std::mutex> lock(m_TextMutex);
	}

	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/E3_Title/tex/E3_Title_BG01.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Logo_Mark */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo_Mark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/E3_Title/tex/LOC/E3_Title_Logo.png")))))
		return E_FAIL;

	//VideoTex
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_LogoVideoTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Video/LogoMultyTexture/Video_LogoTex%d.png"), 145))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CUI_Logo_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Logo"),
		CUI_Logo_Mark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Texture_Resources_GamePlay_0()
{
	{
		std::lock_guard<std::mutex> lock(m_TextMutex);
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	}


	//공격 버프

	/* For.Prototype_Component_Texture_UI_AttBufNone */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBufNone"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/AttBufEffect/cp_spicon_none.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_AttBuf */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBuf"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/AttBufEffect/cp_spicon0%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_AttBufEff */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBufEff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/AttBufEffect/cp_spicon_eff0%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_AttBufMark */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBufMark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/AttBufEffect/cp_spicon_S.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_AttBufElectricEff */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttBufElectricEff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/AttBufEffect/ElectricEff/cp_spicon_ElectricEff%d.png"), 6))))
		return E_FAIL;

	//게임 스타트 
	/* For.Prototype_Component_Texture_UI_GameStartCircle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameStartCircle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Middle/GameStart/GameStart%d.png"), 8))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_GameStartEmblem */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameStartEmblem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Middle/GameStart/Emblem%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_GameReadyFont */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameReadyFont"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Middle/Font/Ready%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_GameFightFont */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameFightFont"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Middle/Font/Fight%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_GameKOFont */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameKOFont"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Middle/Font/BP_roundfinish_KO.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_GameKOFontEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GameKOFontEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Middle/Font/BP_roundfinish_KO_eff.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_KOPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_KOPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/BP_roundfinish_eff0%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_KOParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_KOParticle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/BP_roundfinish_KO_eff_sumi.png")))))
		return E_FAIL;

	//HP




	//Combo
	/* For.Prototype_Component_Texture_UI_ComboNumber */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ComboNumber"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Left/Number/ComboNumber%d.png"), 30))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ComboFont */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ComboFont"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Left/Combo_Hit_Font%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_ComboEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ComboEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Left/cp_combo_eff.png")))))
		return E_FAIL;


	//캐릭터 아이콘

	/* For.Prototype_Component_Texture_UI_CharaIconPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CharaIconPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame//Top/Cp_CharaIconPanel.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CharaIconPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SubCharaIconPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/SubIconPanel%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CharIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CharaIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Chara_Image//Face_A/Chara_Icon/CharaIcon%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_HpPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HpPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/cp_gauge_physical_frame01_S3.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_HpGauge*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HpGauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/HpGauge.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_HpGaugeAnimMask*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HpGaugeAnimMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/cp_lifegauge_animmask.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_SubHp*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SubHp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/cp_gauge_physical_frame01.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_HpEffect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HpEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/cp_lifegauge_base.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_SubHpPanel*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SubHpPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Subphysical_frame.png")))))
		return E_FAIL;

	//시간 초

	/* For.Prototype_Component_Texture_UI_Timer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Timer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/TimerNumber/TimerNum%d.png"), 11))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Time_Panel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_TimerPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/cp_gauge_Timebase.png")))))
		return E_FAIL;

	//스킬 게이지

	/* For.Prototype_Component_Texture_UI_SKillGauge */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SKillGauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/cp_tensiontex_base00_S3.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_SKillGaugeBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SKillGaugeBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Cp_Tensiontex/cp_tensiontex_0%d.png"), 8))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_SPNum */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SPNum"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/SP_Number/SP_Num%d.png"), 8))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_SkillFlowEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SkillFlowEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/cp_lifegauge_animmask_S3.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_SkillEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SkillEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/cp_tension_eff.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_SkillGaugeEff */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SkillGaugeEff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/cp_tensiontex_base00_Eff.png")))))
		return E_FAIL;

	//Win
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Win_Font"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Left/Win_text_win_0%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Team */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Team"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Win/Tex/Team%d.png"), 2))))
		return E_FAIL;

#pragma region Effect Texture

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_Default_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/Default_Mask.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_Default_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/Default_Diffuse.dds"), 1))))
		return E_FAIL;
	//디폴트

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_bun_402smoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/bun_402smoke.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_bun_431tunder00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/bun_431tunder00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_bun_cookie"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/bun_cookie.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_bun431_cseff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/bun431_cseff.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura_end"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura_end.dds"), 1))))
		return E_FAIL;
	// 5

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_htn407_hibi01_W"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/htn407_hibi01_W.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura03.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura03_end"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura03_end.dds"), 1))))
		return E_FAIL;
	//10

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura03_middle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura03_middle.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura03_small"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura03_small.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura03color00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura03color00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura05"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura05.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura06"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura06.dds"), 1))))
		return E_FAIL;
	// 15
	return S_OK;
}

HRESULT CLoader::Load_Texture_Resources_GamePlay_1()
{
	{
		std::lock_guard<std::mutex> lock(m_TextMutex);
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다.1"));
	}





	//KeyInput 

	/* For.Prototype_Component_Texture_UI_DirInputPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DirInputPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/KeyInputPanel.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_ActionInputPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_ActionInput"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/KeyInputButton%d.png"), 7))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_DirInputIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DirInputIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/UI_DirInput%d.png"), 9))))
		return E_FAIL;


	/* Prototype_Component_Texture_UI_InputPanel*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_InputPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/TON_Icon_none.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_DirKeyInputEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DirKeyInputEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/window_arrow_key_%d.png"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_InputIconPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_InputIconPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Training_BlackBG_00.png")))))
		return E_FAIL;


	//Font

	/* Prototype_Component_Texture_UI_FontName */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_FontNameGamePlay"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Chara_Name%d.png"), 4))))
		return E_FAIL;

	//옵션

	/* Prototype_Component_Texture_UI_OptPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CmnMenu/tex/window_bg_01.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_OptArrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptArrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CmnMenu/tex/window_arrow_key_00.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_OptSelectEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptSelectEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CmnMenu/tex/window_arrow_key_01.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_OptVolumePanel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptVolumePanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CmnMenu/tex/window_gage_01.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_OptVolumeGauge */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptVolumeGauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CmnMenu/tex/window_gage_00.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_OptTitle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptTitle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CmnMenu/tex/window_title_00.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_OptSoundFont */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_OptSoundFont"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Sound_Font.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKS_2P"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/GKS_base_2P.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_S21_2P"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/TON_base_2P.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_S21_1P"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/TON_base.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HIT_2P"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/HTN_base_2P.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HIT_1P"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/HTN_base.png"), 1))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_bomb00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_bomb00.dds"), 1))))
		return E_FAIL;
	// 35

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_bomb01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_bomb01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_bomb02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_bomb02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_bomb03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_bomb03.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_bomb04"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_bomb04.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_burst00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_burst00.dds"), 1))))
		return E_FAIL;
	//40

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_burst01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_burst01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_burst01_harf"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_burst01_harf.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_burst02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_burst02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_burst03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_burst03.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_burst04"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_burst04.dds"), 1))))
		return E_FAIL;
	//45

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_burst05"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_burst05.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_burst05_hi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_burst05_hi.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_burst06"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_burst06.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_burst07"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_burst07.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_burst08"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_burst08.dds"), 1))))
		return E_FAIL;
	//50

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_circle00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_circle00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_circle01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_circle01.dds"), 1))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_dis00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_dis00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_distortionA"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_distortionA.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_distortionB"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_distortionB.dds"), 1))))
		return E_FAIL;
	// 55

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_distortionC"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_distortionC.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_dust00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_dust00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_dust01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_dust01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_Fractal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_Fractal.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton409_ray00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton409_ray00.dds"), 1))))
		return E_FAIL;
	//60

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_Fractal2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_Fractal2.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_Fractal3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_Fractal3.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_Fractal3_gray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_Fractal3_gray.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_Fractal4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_Fractal4.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_Fractal5"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_Fractal5.dds"), 1))))
		return E_FAIL;
	// 65

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_Fractal6"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_Fractal6.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_glass00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_glass00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_glass02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_glass02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_gradation_blue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_gradation_blue.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_gradation_orange"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_gradation_orange.dds"), 1))))
		return E_FAIL;
	//70

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_gradation_orange2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_gradation_orange2.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_gradation00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_gradation00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_gradation01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_gradation01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_gradation02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_gradation02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_gradation03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_gradation03.dds"), 1))))
		return E_FAIL;
	// 75

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_grass"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_grass.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_guard00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_guard00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_guard01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_guard01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_guard02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_guard02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_hibana00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_hibana00.dds"), 1))))
		return E_FAIL;
	//80

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_hit00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_hit00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_human00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_human00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_ilm00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_ilm00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_jizokusmoke00end"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_jizokusmoke00end.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_jizokusmoke00end2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_jizokusmoke00end2.dds"), 1))))
		return E_FAIL;
	// 85

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_jizokusmoke00loop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_jizokusmoke00loop.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_jizokusmoke00start"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_jizokusmoke00start.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_kidan00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_kidan00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_kira00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_kira00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_kira01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_kira01.dds"), 1))))
		return E_FAIL;
	//90
	_matrix			PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Loading_GodDragon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Loading_GodDragon.bin", PreTransformMatrix))))
		return E_FAIL;

	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_kmhmFractal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_kmhmFractal.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_line00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_line00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_line01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_line01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PLN_ilm"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Dragon/PLN_ilm.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_space_ending_textrue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/space_ending_textrue.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_line02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_line02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_line03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_line03.dds"), 1))))
		return E_FAIL;
	// 95

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_line04"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_line04.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_line04gray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_line04gray.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_mask00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_mask00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_mask01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_mask01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_mask02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_mask02.dds"), 1))))
		return E_FAIL;
	//100

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_mazzle00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_mazzle00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_mazzle01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_mazzle01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_mazzle02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_mazzle02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_pattern00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_pattern00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_pattern01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_pattern01.dds"), 1))))
		return E_FAIL;
	// 5

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_pattern02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_pattern02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_ray00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_ray00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_ray00new"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_ray00new.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_ray01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_ray01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_rock_PlanetBreak"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_rock_PlanetBreak.dds"), 1))))
		return E_FAIL;
	//10

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_rock_smalls_def"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_rock_smalls_def.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_rock_smalls_space"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_rock_smalls_space.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_rock00_def"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_rock00_def.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_rock01_def"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_rock01_def.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_rock02_def"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_rock02_def.dds"), 1))))
		return E_FAIL;
	// 15

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_samne00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_samne00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_samne01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_samne01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_samne02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_samne02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_scrline00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_scrline00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_scrline01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_scrline01.dds"), 1))))
		return E_FAIL;
	//20

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_scrline02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_scrline02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_scrollsmoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_scrollsmoke.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_slash00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_slash00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke_scroll"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke_scroll.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke_scroll2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke_scroll2.dds"), 1))))
		return E_FAIL;
	// 25

	// 25

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_auradust"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_auradust.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_beamscroll00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_beamscroll00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_BG_earth"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_BG_earth.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_BG_EarthCloud"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_BG_EarthCloud.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_BG_EarthCloud01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_BG_EarthCloud01.dds"), 1))))
		return E_FAIL;
	//30

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_BG_EarthCloud02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_BG_EarthCloud02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_BG_Space"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_BG_Space.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_BG_star01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_BG_star01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_BG_star02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_BG_star02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_QTE_Gauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/Middle/GameStart/Emblem2.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_QTE_Hit_Circle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/CmnBG/tex/CmnBG_Eff_Lens_%d.png"), 8))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_QTE_Result"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/QTE/QTE_RESULT_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_QTE_Arrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/InGame/DebugIcon.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_QTE_Space"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/QTE/QTE_SPACE.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_QTE_Continuous_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/QTE/CC_Congratulations_Eff_02.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura07"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura07.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura08"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura08.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura09"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura09.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura10"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura10.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura11_gray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura11_gray.dds"), 1))))
		return E_FAIL;
	//20

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura12_blue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura12_blue.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura12_gray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura12_gray.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura12_orange"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura12_orange.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura13"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura13.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura14"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura14.dds"), 1))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Texture_Resources_GamePlay_2()
{
	{
		std::lock_guard<std::mutex> lock(m_TextMutex);
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다.2"));
	}

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_spark01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_spark01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_spark02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_spark02.dds"), 1))))
		return E_FAIL;
	//40

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_sphere00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_sphere00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_sphere01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_sphere01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_sss00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_sss00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_stoneanim00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_stoneanim00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_stoneanim00_gray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_stoneanim00_gray.dds"), 1))))
		return E_FAIL;
	//45

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_stoneanim01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_stoneanim01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_stoneanim01gray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_stoneanim01gray.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_stonemodel00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_stonemodel00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_strike00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_strike00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_strike01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_strike01.dds"), 1))))
		return E_FAIL;
	//50

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_strike02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_strike02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_test"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_test.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_thunder00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_thunder00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_thunder01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_thunder01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_thunder02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_thunder02.dds"), 1))))
		return E_FAIL;
	// 55

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_thunder02yellow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_thunder02yellow.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_tigire00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_tigire00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_TinkleLight00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_TinkleLight00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_trail_mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_trail_mask.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_trail00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_trail00.dds"), 1))))
		return E_FAIL;
	//60

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_trail02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_trail02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_trail04"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_trail04.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_water00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_water00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_water01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_water01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_watersplash00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_watersplash00.dds"), 1))))
		return E_FAIL;
	// 65

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_watersplash01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_watersplash01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_wave00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_wave00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_white"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_white.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_wind01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_wind01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_thunder01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_thunder01.dds"), 1))))
		return E_FAIL;
	//70

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_wipe00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_wipe00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_wave01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_wave01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_wipe01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_wipe01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmnsmokemodel00color"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmnsmokemodel00color.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmnsmokemodel02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmnsmokemodel02.dds"), 1))))
		return E_FAIL;
	// 75

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmnsmokemodel02color"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmnsmokemodel02color.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmnsmokemodel03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmnsmokemodel03.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmnsmokemodel03color"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmnsmokemodel03color.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_GKS_cookie"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/GKS_cookie.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_GKS600_cseff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/GKS600_cseff.dds"), 1))))
		return E_FAIL;
	//80

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_GKS600_cseff2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/GKS600_cseff2.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_GKS805_cseff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/GKS805_cseff.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_GKS808_changeScr"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/GKS808_changeScr.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_GKS808_changeScr2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/GKS808_changeScr2.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_gks814_scrwipe"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/gks814_scrwipe.dds"), 1))))
		return E_FAIL;
	// 85

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_htn_burst01_middle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/htn_burst01_middle.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_htn_burst01_smal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/htn_burst01_smal.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_HTN_cookie"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/HTN_cookie.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_htn_gradation01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/htn_gradation01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_htn_gradation02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/htn_gradation02.dds"), 1))))
		return E_FAIL;
	//90

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_htn_hibi01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/htn_hibi01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_htn407_hibi01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/htn407_hibi01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_htn407_hibi02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/htn407_hibi02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_htn407_hibi03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/htn407_hibi03.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_htn431_hibi01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/htn431_hibi01.dds"), 1))))
		return E_FAIL;
	// 95

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_htn431_hibi02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/htn431_hibi02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_sp_EarthCloud"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/sp_EarthCloud.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_sp_EarthCloud01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/sp_EarthCloud01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_sp_EarthCloud02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/sp_EarthCloud02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_sp_star01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/sp_star01.dds"), 1))))
		return E_FAIL;
	//200

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_sp_star02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/sp_star02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_431kidou"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_431kidou.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_431tunder00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_431tunder00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_beam_mask_01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_beam_mask_01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_cookie"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_cookie.dds"), 1))))
		return E_FAIL;
	// 205

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_gradation_01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_gradation_01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_kmhmFractal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_kmhmFractal.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_line00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_line00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_mask_01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_mask_01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_slash01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_slash01.dds"), 1))))
		return E_FAIL;
	//210

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_spark01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_spark01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_ton_thunder00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/ton_thunder00.dds"), 1))))
		return E_FAIL;
	//212

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_T_Spark_Core"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/T_Spark_Core.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_T_Smoke_Tiled_D"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/T_Smoke_Tiled_D.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_T_Smoke_SubUV"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/T_Smoke_SubUV.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_T_Fire_Tiled_D"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/T_Fire_Tiled_D.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_T_Fire_SubUV"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/T_Fire_SubUV.dds"), 1))))
		return E_FAIL;
	// 217

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_T_Explosion_SubUV"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/T_Explosion_SubUV.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_T_Dust_Particle_D"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/T_Dust_Particle_D.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_T_Burst_M"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/T_Burst_M.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_T_Water_N"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/T_Water_N.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_homingB_shadow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_homingB_shadow.dds"), 1))))
		return E_FAIL;
	//222

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_homingB"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_homingB.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_homingAura_stop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_homingAura_stop.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_homingA"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_homingA.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_hit02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_hit02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_hit01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_hit01.dds"), 1))))
		return E_FAIL;
	// 227

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_gradation"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_gradation.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_forcedchange"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_forcedchange.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_dbfly"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_dbfly.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_aura11"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_aura11.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_scrRock01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_scrRock01.dds"), 1))))
		return E_FAIL;
	//232

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_scrRock00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_scrRock00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_ryuhai00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_ryuhai00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_number00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_number00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_homingBair_shadow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_homingBair_shadow.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_homingBair"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_homingBair.dds"), 1))))
		return E_FAIL;
	// 237

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_wind00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_wind00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_scrspark00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_scrspark00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_scrsmoke00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_scrsmoke00.dds"), 1))))
		return E_FAIL;

	//240

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke01.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke02.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke02_blur"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke02_blur.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke03.dds"), 1))))
		return E_FAIL;
	//30

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke04"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke04.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke04b"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke04b.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke05"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke05.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke06"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke06.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke07"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke07.dds"), 1))))
		return E_FAIL;
	// 35

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smoke08"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smoke08.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_smokemodel00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_smokemodel00.dds"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect_cmn_spark00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Eff/Texture/cmn_spark00.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_OutLine"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/GKS_ilm.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TON_decal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/TON_decal.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TON_base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/TON_base.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_21OutLine"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/TON_ilm.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HITOutLine"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/HTN_ilm.png"), 1))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Load_Model_Resources_GamePlay_0()
{
	{
		std::lock_guard<std::mutex> lock(m_TextMutex);
		lstrcpy(m_szLoadingText, TEXT("모델(정점 -> 폴리곤 -> 메시 -> 모델)을 로딩 중 입니다."));
	}
	if (FAILED(Load_Map_Space()))
		return E_FAIL;

	_matrix			PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Hit.bin", PreTransformMatrix))))
		return E_FAIL;*/

		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Goku"),
		//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_SS1.bin", PreTransformMatrix))))
		//	return E_FAIL;


		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Goku"),
		//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_SS1_AllMesh.bin", PreTransformMatrix))))
		//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Goku"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_SS1AllMesh_Event.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_KRN"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Char/KRN/KRN_AllMesh.bin", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_KRN_base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/KRN/KRN_base.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_KRN_decal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/KRN/KRN_decal.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_KRN_ilm"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/KRN/KRN_ilm.png")))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_SS1AllMesh_Event.bin", PreTransformMatrix))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Frieza"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Char/Frieza/FRN_AllMesh.bin", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_base_1P"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Frieza/FRN_base_1P.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_base_2P"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Frieza/FRN_base_2P.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRG_base_1P"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Frieza/FRG_base_1P.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRG_base_2P"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Frieza/FRG_base_2P.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRG_ilm"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Frieza/FRG_ilm.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_ilm"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Frieza/FRN_ilm.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_decal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Frieza/FRN_decal.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_detail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Frieza/FRN_detail.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FRN_metal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/Frieza/metal.png")))))
		return E_FAIL;


	PreTransformMatrix = PreTransformMatrix * XMMatrixRotationX(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKS_decal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/GKS_decal.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKS_base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/GKS_base.png")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKN_base_1P"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/BlackGoku/GKN_base_1P.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKN_base_2P"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/BlackGoku/GKN_base_2P.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKN_ilm"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/BlackGoku/GKN_ilm.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GKN_decal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Char/BlackGoku/GKN_decal.png")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Hit"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Hit.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Ton.bin", PreTransformMatrix))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Goku_Final"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_SS3.bin", PreTransformMatrix))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Goku_Final"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_SS3_AllMesh.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Goku_Opening"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Char/BlackGoku.bin", PreTransformMatrix))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BlackGoku"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Char/BlackGoku.bin", PreTransformMatrix))))
	//	return E_FAIL;


	//21호
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_21"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Ton.bin", PreTransformMatrix))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Ton.bin", PreTransformMatrix))))
	//	return E_FAIL;

	PreTransformMatrix = PreTransformMatrix * XMMatrixRotationX(XMConvertToRadians(180.0f));




#pragma region 주석 리소스 파일

	/*PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);*/

	//손오공 오프닝
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_Opening.bin", PreTransformMatrix))))
	//	return E_FAIL;


	//손오공
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_SS1.bin", PreTransformMatrix))))
		return E_FAIL;*/


		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_Goku"),
		//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_SS1.bin", PreTransformMatrix))))
		//	return E_FAIL;

		//부우
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
		//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Bou.bin", PreTransformMatrix))))
		//	return E_FAIL;


		//히트 선택화면
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
		//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Hit_Select.bin", PreTransformMatrix))))
		//	return E_FAIL;


		//PreTransformMatrix = PreTransformMatrix * XMMatrixRotationX(XMConvertToRadians(180.0f));  //이것도 방향 이상함
	PreTransformMatrix = PreTransformMatrix * XMMatrixRotationX(XMConvertToRadians(180.0f));


	//손오공 3스킬
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_SS3.bin", PreTransformMatrix))))
	//	return E_FAIL;


	//히트
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Hit.bin", PreTransformMatrix))))
	//	return E_FAIL;

	//21호
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Play_21"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Ton.bin", PreTransformMatrix))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Ton.bin", PreTransformMatrix))))
	//	return E_FAIL;



	//21호 선택화면
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Ton_Select.bin", PreTransformMatrix))))
	//	return E_FAIL;

	//부우 선택화면
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Bou_Select.bin", PreTransformMatrix))))
	//	return E_FAIL;

	//오공 선택화면
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_Select.bin", PreTransformMatrix))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_untitled"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Goku_SS3.bin", PreTransformMatrix))))
	//	return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Load_Model_Resources_GamePlay_1()
{
	if (FAILED(Load_Map_Volcano()))
		return E_FAIL;

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);


#pragma region Effect Model

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn301_eff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn301_eff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn403_eff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn403_eff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn403_eff01_sub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn403_eff01_sub.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn405_eff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn405_eff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn405_eff01_sub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn405_eff01_sub.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn406_eff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn406_eff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn406_eff01_sub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn406_eff01_sub.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn407_eff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn407_eff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn407_eff02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn407_eff02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn408_eff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn408_eff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn408_eff02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn408_eff02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn432_eff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn432_eff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn432_eff01_sub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn432_eff01_sub.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_a01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_a01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_a02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_a02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_a03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_a03.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_a04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_a04.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_b01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_b01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_b02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_b02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_b03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_b03.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_b03_sub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_b03_sub.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_b04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_b04.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_b04_sub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_b04_sub.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_c01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_c01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_c01_sub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_c01_sub.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_c02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_c02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_c02_sub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_c02_sub.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_c03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_c03.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_c04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_c04.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_d01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_d01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_d01_sub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_d01_sub.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_d02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_d02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_d02_sub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_d02_sub.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_d03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_d03.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn600cs_eff_hibi_d04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn600cs_eff_hibi_d04.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton_cookie"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton_cookie.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton_kidan01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton_kidan01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton201_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton201_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton204_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton204_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton205_eff1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton205_eff1.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton231_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton231_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton232_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton232_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton301_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton301_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton401_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton401_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton401_eff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton401_eff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton401_eff02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton401_eff02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton401_eff03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton401_eff03.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton401_eff04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton401_eff04.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton430_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton430_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton431_beam"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton431_beam.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton431_eff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton431_eff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton432_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton432_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_ton433_sphere01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/ton433_sphere01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_ryuhai00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_ryuhai00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_homingshere"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_homingshere.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_guardspark00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_guardspark00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_dbfly"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_dbfly.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_circle00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_circle00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_burstCylinder01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_burstCylinder01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_burstCylinder00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_burstCylinder00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_AuraDash01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_AuraDash01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_AuraDash00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_AuraDash00.bin", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0101"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0101.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0102"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0102.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0103"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0103.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0104"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0104.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0105"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0105.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0106"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0106.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0107"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0107.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0108"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0108.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0109"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0109.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0110"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0110.bin", PreTransformMatrix))))
		return E_FAIL;

	//10

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0111"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0111.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0112"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0112.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0201"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0201.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0202"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0202.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0203"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0203.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0204"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0204.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0205"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0205.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0206"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0206.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0301"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0301.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0302"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0302.bin", PreTransformMatrix))))
		return E_FAIL;

	//20

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0303"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0303.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0304"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0304.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0305"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0305.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0306"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0306.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0307"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0307.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0308"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0308.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0309"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0309.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0310"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0310.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0311"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0311.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_SmokeModels0312"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/TestModel/cmn_SmokeModels0312.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_acmn_pivot_plane01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_pivot_plane01.bin", PreTransformMatrix)))) //lying
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_acmn_povot_plane00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_povot_plane00.bin", PreTransformMatrix)))) // stand
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_bun_cookie"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/bun_cookie.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_bun201_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/bun201_eff00.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_bun203_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/bun203_eff00.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_bun204_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/bun204_eff00.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_bun231_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/bun231_eff00.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_bun232_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/bun232_eff00.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_bun261_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/bun261_eff00.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_bun301_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/bun301_eff00.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_bun403_BindLoop"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/bun403_BindLoop.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_bun403_BindStart"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/bun403_BindStart.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_bun403_jizoku"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/bun403_jizoku.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_AuraBall"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_AuraBall.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_BeamCore"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_BeamCore.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_BeamCorePattern"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_BeamCorePattern.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_bodyaura00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_bodyaura00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_crossplane00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_crossplane00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_crossplane01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_crossplane01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_crossplane02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_crossplane02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_cylinder00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_cylinder00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_cylinder01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_cylinder01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_cylinder01_ura"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_cylinder01_ura.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_cylinder02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_cylinder02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_cylinder03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_cylinder03.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_cylinder04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_cylinder04.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_cylinderBeam00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_cylinderBeam00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_cylinderBeam01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_cylinderBeam01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_futtobiBGpoint"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_futtobiBGpoint.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_futtobiBGtemprate"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_futtobiBGtemprate.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_GrassModel"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_GrassModel.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_halfsphere00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_halfsphere00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_halfsphere01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_halfsphere01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_hibana00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_hibana00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_kakusanaura"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_kakusanaura.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_kidan00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_kidan00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_KidanTrail"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_KidanTrail.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_puffycylinder"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_puffycylinder.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_puffyplane"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_puffyplane.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_puffyplane_harf"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_puffyplane_harf.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_puffyplane2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_puffyplane2.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_rock00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_rock00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_rock01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_rock01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_rock02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_rock02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_rock04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_rock04.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_rock05"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_rock05.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_rubble01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_rubble01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_saiyanAura"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_saiyanAura.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_screw"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_screw.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_screw2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_screw2.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_screw3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_screw3.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_shenlongAura00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_shenlongAura00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_shenlongAura01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_shenlongAura01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_shenlongAura02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_shenlongAura02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_sphere00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_sphere00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_sphere00_inv"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_sphere00_inv.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_sphere00_inv2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_sphere00_inv2.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_stonemodel00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_stonemodel00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_strike00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_strike00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_TestStrike"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_TestStrike.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_cmn_WindModel00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/cmn_WindModel00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks_cookie"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks_cookie.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks262_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks262_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks262_eff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks262_eff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks262_eff10"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks262_eff10.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks301_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks301_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks404_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks404_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks404_eff00_sub"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks404_eff00_sub.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks404_eff00_sub2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks404_eff00_sub2.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks406_eff00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks406_eff00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks406_eff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks406_eff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks406_eff02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks406_eff02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks406_eff03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks406_eff03.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks430_tameaura_00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks430_tameaura_00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks431_kaijo"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks431_kaijo.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks432_coremodel"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks432_coremodel.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks432_coremodel2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks432_coremodel2.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks806_sphere00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks806_sphere00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks808_Sphere"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks808_Sphere.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks808_Sphere_inv"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks808_Sphere_inv.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_gks814_fistaura"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/gks814_fistaura.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn_cookie"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn_cookie.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn_halfsphere01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn_halfsphere01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn_plane01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn_plane01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_htn_plane03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/ModelData/Eff/Model/htn_plane03.bin", PreTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Prototype_Object_GamePlay()
{
	{
		std::lock_guard<std::mutex> lock(m_TextMutex);
		lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩 중 입니다."));
	}

#pragma region Effect Object
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_NoneLight"),
		CEffect_NoneLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Blend"),
		CEffect_Blend::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_ZNone"),
		CEffect_ZNone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Overlap"),
		CEffect_Overlap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Preview"),
		CModel_Preview::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Play_Goku"),
		CPlay_Goku::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Opening_Kririn"),
		COpening_Kririn::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Play_21"),
		CPlay_21::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Play_Hit"),
		CPlay_Hit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Play_Frieza"),
		CPlay_Frieza::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Frieza_Metal"),
		CFrieza_Metal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Attack"),
		CAttackObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Attack_Chase"),
		CAttackObject_Chase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Attack_Reflect"),
		CAttackObject_Reflect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Attack_Grab"),
		CAttackObject_Grab::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Attack_CommandGrab"),
		CAttackObject_CommandGrab::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Attack_Ranged"),
		CAttackObject_Ranged::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Attack_Energy"),
		CAttackObject_Energy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ParryingRangedObject"),
		CParryingRangedObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bone_Effect_Object"),
		CBoneEffectObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Stage"),
		CVolcano_Stage::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Cliff_Back"),
		CVolcano_Cliff_Back::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Cliff_Far"),
		CVolcano_Cliff_Far::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Cliff01"),
		CVolcano_Cliff01::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Cliff02"),
		CVolcano_Cliff02::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Cliff03"),
		CVolcano_Cliff03::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Cliff04"),
		CVolcano_Cliff04::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Island01"),
		CVolcano_Island01::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Island02"),
		CVolcano_Island02::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Lava_Fall"),
		CVolcano_Lava_Fall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Lava_Ground"),
		CVolcano_Lava_Ground::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Lava_Pool"),
		CVolcano_Lava_Pool::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Destructive"),
		CVolcano_Destructive::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_GroundRock"),
		CVolcano_GroundRock::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Ground"),
		CVolcano_Ground::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_SkyCloud"),
		CVolcano_SkyCloud::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Smoke"),
		CVolcano_Smoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Volcano_Mountain"),
		CVolcano_Mountain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceMeteoBreak"),
		CSpaceMeteoBreak::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceRock"),
		CSpaceRock::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shadow_Camera"),
		CShadow_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceStone"),
		CSpaceStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceMoon"),
		CSpaceMoon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceSun"),
		CSpaceSun::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceHorizon"),
		CSpaceHorizon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceCliff"),
		CSpaceCliff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceStage"),
		CSpaceStage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceGround"),
		CSpaceGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceEarth_Light"),
		CSpaceEarth_Light::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceEarth"),
		CSpaceEarth::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FallingStar"),
		CFallingStar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceSky"),
		CSpaceSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceEF"),
		CSpaceEF::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VolcanoEF"),
		CVolcanoEF::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shader_Texture"),
		CShader_Texture::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Main_Camera"),
		CMain_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Virtual_Camera"),
		CVirtual_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Line_Draw"),
		CLine_Draw::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Attack"),
		CAttackObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//게임 오브젝트

/* For.Prototype_GameObject_Cursor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Cursor"),
		CUI_Cursor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_HpPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HpPanel"),
		CUI_HpPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_HpGauge */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HpGauge"),
		CUI_HpGauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_SubHpGauge */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_SubHpGauge"),
		CUI_SubHpGauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_HpEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HpEffect"),
		CUI_HpEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_SubHpPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_SubHpPanel"),
		CUI_SubHpPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_UI_Chara_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Chara_Icon_Panel"),
		CUI_Chara_Icon_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Sub_Chara_Icon_Panel */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Sub_Chara_Icon_Panel"),
		CUI_Sub_Chara_Icon_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_Chara_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Chara_Icon"),
		CUI_Chara_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_UI_Chara_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Chara_SubIcon"),
		CUI_Chara_SubIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_AttBufMark */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_AttBufMark"),
		CUI_AttBufMark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_AttBufNone */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_AttBufNone"),
		CUI_AttBufNone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_AttBuf */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_AttBuf"),
		CUI_AttBuf::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_AttBufEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_AttBufEffect"),
		CUI_AttBufEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_AttBufThunderEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_AttBufThunderEffect"),
		CUI_AttBufThunderEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_AttBufIconEff */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_AttBufIconEff"),
		CUI_AttBufIconEff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_SkillGauge */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_SkillGauge"),
		CUI_SkillGauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_SkillGaugeBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_SkillGaugeBar"),
		CUI_SkillGaugeBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_SkillGauge */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_SkillNumber"),
		CUI_SkillNumber::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_SkillEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_SkillEffect"),
		CUI_SkillEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//시간 초

	/* For.Prototype_GameObject_UI_Timer */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Timer"),
		CUI_Timer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_TimerPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TimerPanel"),
		CUI_TimerPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//콤보

	/* For.Prototype_GameObject_UI_ComboNumber */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ComboNumber"),
		CUI_ComboNumber::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_ComboFont */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ComboFont"),
		CUI_ComboFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_ComboEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ComboEffect"),
		CUI_ComboEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//게임 스테이트

	/* For.Prototype_GameObject_UI_ComboEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_GameStartCircle"),
		CUI_GameStartCircle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_ReadyFont */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ReadyFont"),
		CUI_ReadyFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_FightFont */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_FightFont"),
		CUI_FightFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_FightFont */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_StartEmblem"),
		CUI_StartEmblem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_KOFont */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_KOFont"),
		CUI_KOFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_KOFontEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_KOFontEffect"),
		CUI_KOFontEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_KOPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_KOPanel"),
		CUI_KOPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_KOParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_KOParticle"),
		CUI_KOParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_InputDir */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_InputDir"),
		CUI_InputDir::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_InputDirPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_InputDirPanel"),
		CUI_Input_DirPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_InputAction */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_InputAction"),
		CUI_Input_Action::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_DirInputIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_DirInputIcon"),
		CUI_InputDirIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_DirInputIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_InputIconPanel"),
		CUI_Input_IconPanel::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* Prototype_GameObject_UI_ActionInputIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ActionInputIcon"),
		CUI_Input_ActionIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_DirInputEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_DirInputEffect"),
		CUI_InputDirEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_FontName */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_FontName"),
		CUI_FontName::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//옵션

	/* Prototype_GameObject_UI_Opt_Sound_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Opt_Sound_Panel"),
		CUI_Opt_Sound_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Opt_Sound_Arrow*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Opt_Sound_Arrow"),
		CUI_Opt_Sound_Arrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Opt_Sound_Volume_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Opt_Sound_Volume_Panel"),
		CUI_Opt_Sound_Volume_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Opt_Sound_Volume_Gauge*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Opt_Sound_Volume_Gauge"),
		CUI_Opt_Sound_Volume_Gauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Opt_Sound_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Opt_Sound_Effect"),
		CUI_Opt_Sound_Eff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Opt_Sound_Font*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Opt_Sound_Font"),
		CUI_Opt_Sound_Font::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_UI_Opt_Sound_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Opt_Sound_Title"),
		CUI_Opt_Sound_Title::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WIn_Font"),
		CUI_Win_Font::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WIn_Circle"),
		CUI_Win_Circle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WIn_Particle"),
		CUI_Win_Particle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WIn_Team"),
		CUI_Win_Team::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* Prototype_GameObject_UI_WIn_Font */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_WIn_Font"),
	//	CUI_Win_Font::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Same_Grab_UI_Icon"),
		CQTE_Same_Grab_UI_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Same_Grab"),
		CQTE_Same_Grab::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Same_Grab_UI_Gauge"),
		CQTE_Same_Grab_UI_Gauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Hit"),
		CQTE_Hit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Hit_UI_Icon"),
		CQTE_Hit_UI_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Hit_Situation"),
		CQTE_Hit_Situation::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Hit_UI_MovingRing_Icon"),
		CQTE_Hit_UI_MovingRing_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Hit_UI_Result"),
		CQTE_Hit_UI_Result::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Continuous_Attack"),
		CQTE_Continuous_Attack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Continuous_Attack_Space"),
		CQTE_Continuous_Attack_Space::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Continuous_Gauge"),
		CQTE_Continuous_Attack_Gauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Hit_UI_Particle"),
		CQTE_Hit_UI_Particle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Continuous_Attack_Effect"),
		CQTE_Continuous_Attack_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Same_Grab_UI_Particle"),
		CQTE_Same_Grab_UI_Particle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Continuous_Attack_Particle"),
		CQTE_Continuous_Attack_Particle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_Continuous_Attack_Space_Particle"),
		CQTE_Continuous_Attack_Space_Particle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QTE_1P_Same_Grab"),
		CQTE_1P_Same_Grab::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Spread"),
		CParticle_Spread::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Focus"),
		CParticle_Focus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Common_Hit"),
		CParticle_Common_Hit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Frieza_1_Ultimate_Hit"),
		CParticle_Frieza_1_Ultimate_Hit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Frieza_3_Ultimate_Hit"),
		CParticle_Frieza_3_Ultimate_Hit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_21_3_Ultimate"),
		CParticle_21_3_Ultimate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SubTitle"),
		CSubTitle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Map_Space()
{
	_matrix	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

#pragma region SpaceResources
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MeteoBreak"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/MeteoBreak/MeteoBreak.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceEF1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/EF/SpaceEF1.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceEF"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/EF/SpaceEF.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_VolcanoEF"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/EF/VolcanoEF.bin", PreTransformMatrix))))
		return E_FAIL;

	for (size_t i = 0; i < 11; i++)
	{
		wstring strTagName = TEXT("Prototype_Component_Model_MeteoBrake_") + to_wstring(i + 1);
		string strModelName = "../Bin/Resources/Map/Space/MeteoBreak/MeteoBrake_" + to_string(i + 1) + ".bin";
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strTagName.c_str(),
			CModel::Create(m_pDevice, m_pContext, strModelName.c_str(), PreTransformMatrix))))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_sp_meteobrake01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/MeteoBreak/sp_meteobrake01.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SpaceSky_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/SpaceSky/sp_Space.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SpaceSky_Star1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/SpaceSky/sp_star01.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SpaceSky_Star2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/SpaceSky/sp_star02.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_FallingStar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/FallingStar/sp_SStar.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Ground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Ground/sp_ground.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Ground_Cliff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Ground/sp_groundcliff.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Ground_Pattern"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Ground/sp_ground_pattern.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Ground_Shadow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Ground/sp_ground_shadow.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Crater"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Ground/sp_crater.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Stage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/BRGround/sp_BRGround.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Cliff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Cliff/sp_cliff.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Horizon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Horizon/sp_horizon_light.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Light"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Sun/sp_light.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Rainbow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Sun/sp_rainbow.png"), 1))))
		return E_FAIL;



	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Earth/sp_earth.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Light"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Earth/Earth_Light.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Shadow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Earth/Earth_Shadow.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Space_Earth_Cloud"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Space/Earth/Earth_Cloud_%d.png"), 3))))
		return E_FAIL;
	//-----------------------------------//-----------------------------------//-----------------------------------

	/*---------------------------------------- - Map_Space--------------------------------------------*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceGround"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Ground/sp_Ground.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceStage"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/BRGround/BRGround.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceCliff"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Cliff/Cliff.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceHorizon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Horizon/Horizon.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceSun"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Sun/Sun.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceRainbow"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Sun/Rainbow.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceStone"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Stone/Stone.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceSky"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/SpaceSky/SpaceSky.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FallingStar"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/FallingStar/FallingStar.bin", PreTransformMatrix))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceEarth"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Earth/Earth.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceMoon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Moon/Moon.bin", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceRock_1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Rock_1/Rock_1.bin", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceRock_2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Rock_2/Rock_2.bin", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceRock_3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Rock_3/Rock_3.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceBRRock_1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Rock_1/BRRock_1.bin", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceBRRock_2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Rock_2/BRRock_2.bin", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceBRRock_3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Rock_3/BRRock_3.bin", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceMeteo_1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Meteo_1/Meteo_1.bin", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceMeteo_2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Meteo_2/Meteo_2.bin", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SpaceMeteo_3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Space/Meteo_3/Meteo_3.bin", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Load_Map_Volcano()
{
	/*---------------------------------------- - Map_Volcano--------------------------------------------*/

/*---------------------------------- -//-----------------TEXTURE------------------//-----------------------------------*/


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_vo_smoke02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Volcano/vo_smoke03/vo_smoke02.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_vo_BRground01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Volcano/vo_BRground/vo_BRground01.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_vo_skycloud2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Volcano/vo_skycloud/vo_skycloud2.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_vo_river01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Volcano/vo_lava_ground/vo_river01.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_vo_river02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Volcano/vo_lava_ground/vo_river02.png"), 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_vo_river03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Volcano/vo_lava_ground/vo_river03.png"), 1))))
		return E_FAIL;
	/*---------------------------------- -//-----------------MODEL------------------//-----------------------------------*/

	_matrix	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_smoke03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_smoke03/vo_smoke03.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_skycloud"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_skycloud/vo_skycloud.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_mountain"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_mountain/vo_mountain.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_DestructiveFinish"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_DestructiveFinish/vo_DestructiveFinish.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_SFmountain01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_DestructiveFinish/vo_SFmountain01.bin", PreTransformMatrix))))
		return E_FAIL;

	for (size_t i = 0; i < 15; i++)
	{
		wstring strTagName = TEXT("Prototype_Component_Model_vo_SFmountainrRock") + to_wstring(i);
		string strModelName = "../Bin/Resources/Map/Volcano/vo_DestructiveFinish/vo_SFmountainrRock" + to_string(i) + ".bin";
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strTagName.c_str(),
			CModel::Create(m_pDevice, m_pContext, strModelName.c_str(), PreTransformMatrix))))
			return E_FAIL;
	}



	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_lava_pool"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_lava_pool/vo_lava_pool.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_lava_ground"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_lava_ground/vo_lava_ground.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_lava_fall"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_lava_fall/vo_lava_fall.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_island02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_island02/vo_island02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_island01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_island01/vo_island01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_groundrock"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_groundrock/vo_groundrock.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_ground00"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_ground00/vo_ground00.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_cliff04"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_cliff04/vo_cliff04.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_cliff03"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_cliff03/vo_cliff03.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_cliff02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_cliff02/vo_cliff02.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_cliff01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_cliff01/vo_cliff01.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_cliff_far"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_cliff_far/vo_cliff_far.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_cliff_back"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_cliff_back/vo_cliff_back.bin", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_vo_BRground"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Map/Volcano/vo_BRground/vo_BRground.bin", PreTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Prototype_Component_GamePlay()
{
	{
		std::lock_guard<std::mutex> lock(m_TextMutex);
		lstrcpy(m_szLoadingText, TEXT("컴포넌트를 로딩 중 입니다."));
	}

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxSpace */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSpace"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxSpace.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxSpace */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxVolcano"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxVolcano.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxShaderRect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxShaderRect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

#pragma region QTE 파티클

	//동시잡기 파티클
	CVIBuffer_Instancing::VIBUFFER_INSTANCE_DESC	ParticleDesc{};
	ParticleDesc.iNumInstance = 200;
	ParticleDesc.vRange = _float3(1.f, 1.f, 0.f);
	ParticleDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ParticleDesc.vPivot = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vSpeed = _float2(5.f, 7.f);
	ParticleDesc.vScale = _float2(4.f, 4.f);
	ParticleDesc.vLifeTime = _float2(0.1f, 0.2f);
	ParticleDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Same_Grab_Spread_QTE"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	//연타 파티클
	ParticleDesc.iNumInstance = 200;
	ParticleDesc.vRange = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ParticleDesc.vPivot = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vSpeed = _float2(0.2f, 0.5f);
	ParticleDesc.vScale = _float2(2.f, 2.f);
	ParticleDesc.vLifeTime = _float2(0.05f, 0.1f);
	ParticleDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Continuous_Spread_QTE"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	//연타 스페이스 파티클
	ParticleDesc.iNumInstance = 200;
	ParticleDesc.vRange = _float3(1.f, 1.f, 0.f);
	ParticleDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ParticleDesc.vPivot = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vSpeed = _float2(3.f, 5.f);
	ParticleDesc.vScale = _float2(2.f, 2.f);
	ParticleDesc.vLifeTime = _float2(0.2f, 0.3f);
	ParticleDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Continuous_Space_Spread_QTE"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	//Hit 파티클
	ParticleDesc.iNumInstance = 500;
	ParticleDesc.vRange = _float3(0.1f, 0.1f, 0.f);
	ParticleDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ParticleDesc.vPivot = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vSpeed = _float2(0.05f, 0.3f);
	ParticleDesc.vScale = _float2(1.5f, 1.5f);
	ParticleDesc.vLifeTime = _float2(0.6f, 0.8f);
	ParticleDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Hit_Spread_QTE"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;
#pragma endregion

#pragma region 노말 파티클

	//프리저 1필 Focus 파티클
	ParticleDesc.iNumInstance = 70000;
	ParticleDesc.vRange = _float3(20.f, 2.f, 20.f);
	ParticleDesc.vCenter = _float3(0.0f, -2.f, 0.0f);
	ParticleDesc.vPivot = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vSpeed = _float2(2.f, 3.f);
	ParticleDesc.vScale = _float2(0.01f, 0.02f);
	ParticleDesc.vLifeTime = _float2(1.5f, 1.8f);
	ParticleDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Frieza_FocusPoint"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	//프리저 3필 Spread 파티클
	ParticleDesc.iNumInstance = 50000;
	ParticleDesc.vRange = _float3(0.01f, 0.01f, 0.f);
	ParticleDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ParticleDesc.vPivot = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vSpeed = _float2(2.f, 3.f);
	ParticleDesc.vScale = _float2(0.01f, 0.02f);
	ParticleDesc.vLifeTime = _float2(1.0f, 1.5f);
	ParticleDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Spread"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	//프리저 1필 Spread  Hit 파티클
	ParticleDesc.iNumInstance = 5000;
	ParticleDesc.vRange = _float3(5.f, 1.f, 1.f);
	ParticleDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ParticleDesc.vPivot = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vSpeed = _float2(12.f, 15.f);
	ParticleDesc.vScale = _float2(0.03f, 0.04f);
	ParticleDesc.vLifeTime = _float2(0.3f, 0.5f);
	ParticleDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Freiza_Ultimate_1_Hit_Spread"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	//프리저 3필 Spread  Hit 파티클
	ParticleDesc.iNumInstance = 20000;
	ParticleDesc.vRange = _float3(2.f, 1.f, 1.f);
	ParticleDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ParticleDesc.vPivot = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vSpeed = _float2(12.f, 15.f);
	ParticleDesc.vScale = _float2(0.03f, 0.04f);
	ParticleDesc.vLifeTime = _float2(0.3f, 0.5f);
	ParticleDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Freiza_Ultimate_3_Hit_Spread"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	//21 3필 Spread 파티클
	ParticleDesc.iNumInstance = 20000;
	ParticleDesc.vRange = _float3(1.f, 1.f, 1.f);
	ParticleDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ParticleDesc.vPivot = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vSpeed = _float2(6.f, 8.f);
	ParticleDesc.vScale = _float2(0.03f, 0.04f);
	ParticleDesc.vLifeTime = _float2(1.0f, 2.f);
	ParticleDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_21_Ultimate_3_21_Spread"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;

	//공통 Hit Spread 파티클
	ParticleDesc.iNumInstance = 500.f;
	ParticleDesc.vRange = _float3(1.f, 1.f, 1.f);
	ParticleDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ParticleDesc.vPivot = _float3(0.0f, 0.0f, 0.f);
	ParticleDesc.vSpeed = _float2(5.f, 7.f);
	ParticleDesc.vScale = _float2(0.01f, 0.02f);
	ParticleDesc.vLifeTime = _float2(0.2f, 0.3f);
	ParticleDesc.isLoop = false;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Hit_Spread"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ParticleDesc))))
		return E_FAIL;
#pragma endregion


	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Particle_VtxPoint"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle_VtxPoint.hlsl"), VTXPARTICLE_POINT::Elements, VTXPARTICLE_POINT::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_QTE_VtxRect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_QTE_VtxRect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
