#include "Loader.h"

#include <chrono>
#include <sstream>

#include "Game_PresetTable.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Monster.h"
#include "Battle_Pokemon.h"
#include "ForkLift.h"
#include "Player.h"
#include "Body_Player.h"
#include "Weapon.h"
#include "Sky.h"
#include "MapObject.h"
#include "Player_LGPE.h"
#include "Body_Hero.h"
#include "Snow.h"
#include "Explosion.h"
#include "VIBuffer_UI_Instance.h"
#include "Effect_Star.h"
#include "Battle_Trainer.h"
#include "Body_Pokemon.h"
#include "RenderRule_Manager.h"
#include "UIImage_FadeBattle.h"
#include "Body_Human.h"
#include "Actor_NPC.h"
#include "Actor_WildPokemon.h"
#include "Actor_CaptureTarget.h"
#include "Interaction_Dialogue.h"
#include "Interaction_Encounter.h"
#include "Interaction_BallHit.h"
#include "MonsterBall.h"
#include "CaptureRing.h"

#include "UIContainer.h"
#include "UIImage.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIProgressBar.h"

#include "GameInstance.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY ThreadMain(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);
	auto* pTaskQueue = pLoader->Get_TaskQueue();
	const HRESULT hrCoInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

#ifdef _DEBUG
	/* 워커 다수가 동시에 wcout 에 쓰면 줄이 섞이므로 static mutex 로 줄 단위 직렬화 */
	static std::mutex s_LogMutex;
#endif

	CLoader::LOAD_TASK task;
	while (pTaskQueue->try_pop(task))
	{
#ifdef _DEBUG
		const auto t0 = chrono::steady_clock::now();
#endif

		const HRESULT hrTask = task.fn();

#ifdef _DEBUG
		const auto t1 = chrono::steady_clock::now();
		const auto ms = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count();

		{
			/* 로그 문자열은 로컬에서 조립 (race 없음) → mutex 보호는 콘솔 쓰기에만 한정 */
			wstringstream wss;
			wss << L"[LOAD] [TID=" << GetCurrentThreadId() << L"] "
					<< (FAILED(hrTask) ? L"[F] " : L"[S] ")
					<< task.strDebugName
					<< L" : " << ms << L"ms\n";

			const _wstring str = wss.str();

			std::lock_guard<std::mutex> lk(s_LogMutex);
			DWORD written = 0;
			WriteConsoleW(
					GetStdHandle(STD_OUTPUT_HANDLE),
					str.c_str(),
					static_cast<DWORD>(str.size()),
					&written,
					nullptr);
	}
#endif

		if (FAILED(hrTask))
		{
			pLoader->Set_ErrorTask(task.strDebugName);
			pLoader->Set_Error(true);
		}

		pLoader->Add_Progress();
	}

	if (SUCCEEDED(hrCoInit))
		CoUninitialize();

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;
	Enqueue_All(eNextLevelID);	// 큐 적재 + Total 계산 : 메인스레드 단독

	_uint iHW = max(2u, thread::hardware_concurrency());
	//_uint iWorkerCount = max(1u, min(iHW - 1u, iHW * 2 / 3));
	_uint iWorkerCount = max(1u, iHW);
	m_Threads.resize(iWorkerCount, nullptr);

	for (size_t i = 0; i < m_Threads.size(); ++i)
	{
		HANDLE handle = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr));

		if (nullptr == handle)
		{
			for (size_t j = 0; j < i; ++j)
			{
				WaitForSingleObject(m_Threads[j], INFINITE);
				CloseHandle(m_Threads[j]);
				m_Threads[j] = nullptr;
			}

			m_Threads.clear();
			return E_FAIL;
		}

		m_Threads[i] = handle;
	}
	return S_OK;
}

void CLoader::Set_ErrorTask(const _wstring& strTaskName)
{
	lock_guard<mutex> lock(m_ErrorMutex);

	if (m_strLastErrorTask.empty())
		m_strLastErrorTask = strTaskName;
}

_wstring CLoader::Get_LastErrorTask() const
{
	lock_guard<mutex> lock(m_ErrorMutex);
	return m_strLastErrorTask;
}

#ifdef _DEBUG

void CLoader::Show()
{
	_wstring strLoadText =	to_wstring(m_iCompletedCount.load()) + L" / "
							+ to_wstring(m_iTotalCount) + L" ("
							+ to_wstring(m_Threads.size()) + L"개 스레드 가동 중)";

	const _wstring strLastErrorTask = Get_LastErrorTask();
	if (Has_Error() && false == strLastErrorTask.empty())
	{
		strLoadText += L" | Failed: ";
		strLoadText += strLastErrorTask;
	}

	SetWindowText(m_pGameInstance->Get_HWND(), strLoadText.c_str());
}

#endif

void CLoader::Enqueue_LoadTask(TaskFunc fn, const _tchar* pDebugName)
{
	LOAD_TASK tTask = {};
	tTask.strDebugName = (nullptr != pDebugName) ? pDebugName : TEXT("Load Task");
	tTask.fn = move(fn);

	m_TaskQueue.push(move(tTask));
	++m_iTotalCount;
}

template<typename Factory>
inline void CLoader::Enqueue_Prototype(_uint iLevelIndex, WNameID strProtoTag, Factory FactoryFunc, const _tchar* pDebugName)
{
	if (m_pGameInstance->Has_Prototype(iLevelIndex, strProtoTag))
		return;

	Enqueue_LoadTask(
		[this, iLevelIndex, strProtoTag, FactoryFunc = move(FactoryFunc)]() mutable
		{
			return m_pGameInstance->Add_Prototype(iLevelIndex, strProtoTag, FactoryFunc());
		},
		pDebugName);
}

void CLoader::Enqueue_All(LEVEL eNextLevelID)
{
	switch (eNextLevelID)
	{
	case LEVEL::LOGO:
		Ready_Resources_For_Logo();
		break;

	case LEVEL::GAMEPLAY:
		Ready_Resources_For_GamePlay();
		break;

	default:
		break;
	}
}

HRESULT CLoader::Ready_Resources_For_Logo()
{
	// ---------- Texture ----------
	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEX_TITLE_PBGF_DIFF,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/title/title_pbgf_00.png"), 1); },
		TEXT("Prototype_Component_Texture_Title_pbgf_Diff"));

	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEX_TITLE_LOGO_DIFF,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/title/title_logo_%02d.png"), 3); },
		TEXT("Prototype_Component_Texture_Title_Logo_Diff"));

	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEX_TITLE_PBTN_DIFF,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/title/title_pbtn_%02d.png"), 1); },
		TEXT("Prototype_Component_Texture_Title_pbtn_Diff"));

	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEX_TITLE_PIKA,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/title/pber_pika_%02d.png"), 11); },
		TEXT("Prototype_Component_Texture_Title_Pika"));

	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEX_STAR,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Star/Star_%02d.png"), 3); },
		TEXT("Prototype_Component_Texture_Star"));

	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEX_TITLE_BG,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/title/Title_BG.png"), 1); },
		TEXT("Prototype_Component_Texture_Title_BackGround"));

	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEX_BACKGROUND,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Default%d.jpg"), 2); },
		TEXT("Prototype_Component_Texture_BackGround"));

	// ---------- VIBuffer ----------
	CVIBuffer_UI_Instance::UI_INSTANCE_DESC StarDesc{};
	StarDesc.iNumInstance = 64;
	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_VIBUFFER_INST_STAR,
		[this, StarDesc]() mutable { return CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &StarDesc); },
		TEXT("Prototype_Component_VIBuffer_Instance_Star"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0025_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0025_00/pm0025_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0025_00"));

	// ---------- Shader ----------
	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_SHADER_VTXUIINST,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxUIInstance.hlsl"), VTXUI_INSTANCE_DESC::Elements, VTXUI_INSTANCE_DESC::iNumElements); },
		TEXT("Prototype_Component_Shader_VtxUIInstance"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_POKEMON,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Pokemon.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements); },
		TEXT("Prototype_Component_Shader_Pokemon"));

	// ---------- GameObject ----------
	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_OBJ_TITLE_BG,
		[this] { return CBackGround::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_BackGround"));

	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_OBJ_EFT_STAR,
		[this] { return CEffect_Star::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Effect_Star"));

	Enqueue_Prototype(ETOUI(LEVEL::LOGO), PROTO_OBJ_LOGO_MONSTER,
		[this] { return CMonster::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Logo_Monster"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_CAMERA_FREE,
		[this] { return CCamera_Free::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Camera_Free"));

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlay()
{
	// ---------- Texture ----------
	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_TEX_CURSOR,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/cursor/cursor.png"), 1); },
		TEXT("Prototype_Component_Texture_Cursor"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_MENU_BALL,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/mainmenu/main_menu_ball.png"), 1); },
		TEXT("Prototype_Component_Texture_Menu_Ball"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_MENU_PARTNER,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/mainmenu/main_menu_partner_%02d.png"), 3); },
		TEXT("Prototype_Component_Texture_Menu_Partner"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_MENU_DEX,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/mainmenu/main_menu_dex_%02d.png"), 5); },
		TEXT("Prototype_Component_Texture_Menu_Dex"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_MENU_BAG,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/mainmenu/main_menu_bag_%02d.png"), 5); },
		TEXT("Prototype_Component_Texture_Menu_Bag"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_MENU_ENTRY,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/mainmenu/main_menu_entry_%02d.png"), 5); },
		TEXT("Prototype_Component_Texture_Menu_Entry"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_MENU_LINK,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/mainmenu/main_menu_link_%02d.png"), 5); },
		TEXT("Prototype_Component_Texture_Menu_Link"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_MENU_REPORT,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/mainmenu/main_menu_report_%02d.png"), 5); },
		TEXT("Prototype_Component_Texture_Menu_Report"));




	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_FADE_COL,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Fade_Battle/trainer_bg_col.png"), 1); },
		TEXT("Prototype_Component_Texture_Battle_Fade_Color"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_FADE_NOISE,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Fade_Battle/trainer_bg_noise.png"), 1); },
		TEXT("Prototype_Component_Texture_Battle_Fade_Noise"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_FADE_OUT,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Fade_Battle/trainer_bg_out.png"), 1); },
		TEXT("Prototype_Component_Texture_Battle_Fade_Out"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_FADE_LINE,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Fade_Battle/trainer_line.png"), 1); },
		TEXT("Prototype_Component_Texture_Battle_Fade_Line"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_FADE_MASK,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Fade_Battle/trainer_ptn_%02d.png"), 39); },
		TEXT("Prototype_Component_Texture_Battle_Fade_Mask"));
	


	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_ENTRY_BG_PLATE,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/entry/Entry_BG_Plate_%02d.png"), 4); },
		TEXT("Prototype_Component_Texture_Entry_BG_Plate"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_ENTRY_ICON,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/entry/Entry_Icon.png"), 1); },
		TEXT("Prototype_Component_Texture_Entry_Icon"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_ENTRY_PLATE,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/entry/Entry_Plate_%02d.png"), 3); },
		TEXT("Prototype_Component_Texture_Entry_Plate"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_POKEMON_ICON,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/icon_pokemon/poke_icon_%03d.png"), 154); },
		TEXT("Prototype_Component_Texture_Pokemon_Icon"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_LOADING_MARK,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/loading/gokigen_mark_p_%02d.png"), 15); },
		TEXT("Prototype_Component_Texture_Loading_Mark"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_MSGBOX_BG,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/msgBox/msgBox_BG_%02d.png"), 2); },
		TEXT("Prototype_Component_Texture_MsgBox_BG"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_MSGBOX_ICON,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/msgBox/msgBox_Icon.png"), 1); },
		TEXT("Prototype_Component_Texture_MsgBox_Icon"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_STATUS_GAUGE_BACK,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/status/gauge_back.png"), 1); },
		TEXT("Prototype_Component_Texture_Status_Gauge_Back"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_STATUS_GAUGE_FRAME,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/status/gauge_frame.png"), 1); },
		TEXT("Prototype_Component_Texture_Status_Gauge_Frame"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_STATUS_GAUGE_OUTLINE,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/status/gauge_outline.png"), 1); },
		TEXT("Prototype_Component_Texture_Status_Gauge_Outline"));



	// ---------- Shader ----------
	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXNORTEX,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements); },
		TEXT("Prototype_Component_Shader_VtxNorTex"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXMESH,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements); },
		TEXT("Prototype_Component_Shader_VtxMesh"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXANIMMESH,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements); },
		TEXT("Prototype_Component_Shader_VtxAnimMesh"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXCUBE,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements); },
		TEXT("Prototype_Component_Shader_VtxCube"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXRECTINST,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECT_INSTANCE_DESC::Elements, VTXRECT_INSTANCE_DESC::iNumElements); },
		TEXT("Prototype_Component_Shader_VtxRectInstance"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXPOINTINST,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINT_INSTANCE_DESC::Elements, VTXPOINT_INSTANCE_DESC::iNumElements); },
		TEXT("Prototype_Component_Shader_VtxPointInstance"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_PLAYER_LGPE,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Player_LGPE.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements); },
		TEXT("Prototype_Component_Shader_Player_LGPE"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_HUMAN,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Human.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements); },
		TEXT("Prototype_Component_Shader_Human"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_MAP,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Map.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements); },
		TEXT("Prototype_Component_Shader_Map"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_UIIMAGE,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_UIImage.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements); },
		TEXT("Prototype_Component_Shader_UIImage"));



	// ---------- VIBuffer ----------
	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_CUBE,
		[this] { return CVIBuffer_Cube::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_Component_VIBuffer_Cube"));



	// ---------- Model ----------
	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0001_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0001_00/pm0001_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0001_00"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0004_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0004_00/pm0004_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0004_00"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0007_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0007_00/pm0007_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0007_00"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0010_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0010_00/pm0010_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0010_00"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0041_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0041_00/pm0041_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0041_00"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0043_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0043_00/pm0043_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0043_00"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0059_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0059_00/pm0059_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0059_00"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0074_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0074_00/pm0074_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0074_00"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0095_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0095_00/pm0095_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0095_00"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0121_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0121_00/pm0121_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0121_00"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PM0130_00,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/pkm/PM0130_00/pm0130_00.wmodel"); },
		TEXT("Prototype_Component_Model_PM0130_00"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_HERO,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/Hero/tr0001_00.wmodel"); },
		TEXT("Prototype_Component_Model_Hero"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_DOCTOR,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/people/doctor/doctor.wmodel"); },
		TEXT("Prototype_Component_Model_Doctor"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PPL_ROCK,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/people/rock/rock.wmodel"); },
		TEXT("Prototype_Component_Model_People_Rock"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PPL_WATER,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/people/water/water.wmodel"); },
		TEXT("Prototype_Component_Model_People_Water"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PPL_FAT,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/people/fat/fat.wmodel"); },
		TEXT("Prototype_Component_Model_People_Fat"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_MAP_TOWN01,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/LGPE_Map/area02/town01_2.wmodel"); },
		TEXT("Prototype_Component_Model_Town01"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_MAP_TOWN02,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/LGPE_Map/area03/town_02.wmodel"); },
		TEXT("Prototype_Component_Model_Town02"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_MAP_ROAD01,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/LGPE_Map/area02/road01.wmodel"); },
		TEXT("Prototype_Component_Model_Road01"));



	// ---------- Navigation & Collider ----------
	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_NAVIGATION_MAP,
		[this] { return CNavigation::Create(m_pDevice, m_pContext, TEXT("../../DataFiles/MapNaviMesh.nav")); },
		TEXT("Prototype_Component_Navigation_Map"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_AABB,
		[this] { return CCollider::Create(m_pDevice, m_pContext, COLLIDER::AABB); },
		TEXT("Prototype_Component_Collider_AABB"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_OBB,
		[this] { return CCollider::Create(m_pDevice, m_pContext, COLLIDER::OBB); },
		TEXT("Prototype_Component_Collider_OBB"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_SPHERE,
		[this] { return CCollider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE); },
		TEXT("Prototype_Component_Collider_Sphere"));

	// ---------- Interaction ----------
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_INTERACTION_DIALOGUE,
		[this] { return CInteraction_Dialogue::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_Component_Interaction_Dialogue"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_INTERACTION_ENCOUNTER,
		[this] { return CInteraction_Encounter::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_Component_Interaction_Encounter"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_INTERACTION_BALLHIT,
		[this] { return CInteraction_BallHit::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_Component_Interaction_BallHit"));

	// ---------- Objects ----------
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_PLAYER_LGPE,
		[this] { return CPlayer_LGPE::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Player_LGPE"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_BODY_HERO,
		[this] { return CBody_Hero::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Body_Hero"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_BODY_POKEMON,
		[this] { return CBody_Pokemon::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Body_Pokemon"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_BODY_HUMAN,
		[this] { return CBody_Human::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Body_Human"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_ACTOR_NPC,
		[this] { return CActor_NPC::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Actor_NPC"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_ACTOR_WILD_POKEMON,
		[this] { return CActor_WildPokemon::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Actor_WildPokemon"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_IMG_FADEBATTLE,
		[this] { return CUIImage_FadeBattle::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_UIImage_FadeBattle"));

	CMapObject::MAPOBJECT_DESC tMapDesc{};
	tMapDesc.iModelLevelIndex = ETOUI(LEVEL::GAMEPLAY);

	tMapDesc.strModelTag = PROTO_COM_MODEL_MAP_TOWN01;
	tMapDesc.pRenderRule = CRenderRule_Manager::GetInstance()->Find_OrLoadMappingRule("../../Resources/LGPE_Map/area02/town01_2_mapping.json");
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_MAP_TOWN01,
		[this, tMapDesc] { return CMapObject::Create(m_pDevice, m_pContext, tMapDesc); },
		TEXT("Prototype_MapObject_Town01"));

	tMapDesc.strModelTag = PROTO_COM_MODEL_MAP_TOWN02;
	tMapDesc.pRenderRule = CRenderRule_Manager::GetInstance()->Find_OrLoadMappingRule("../../Resources/LGPE_Map/area03/town_02_mapping.json");
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_MAP_TOWN02,
		[this, tMapDesc] { return CMapObject::Create(m_pDevice, m_pContext, tMapDesc); },
		TEXT("Prototype_MapObject_Town02"));

	tMapDesc.strModelTag = PROTO_COM_MODEL_MAP_ROAD01;
	tMapDesc.pRenderRule = CRenderRule_Manager::GetInstance()->Find_OrLoadMappingRule("../../Resources/LGPE_Map/area02/road01_mapping.json");
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_MAP_ROAD01,
		[this, tMapDesc] { return CMapObject::Create(m_pDevice, m_pContext, tMapDesc); },
		TEXT("Prototype_MapObject_Road01"));



#pragma region STUDY
	// Texture
	/* Prototype_Component_Texture_Sky */
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_SKY,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/SkyBox/Sky_%d.dds"), 4); },
		TEXT("Prototype_Component_Texture_Sky"));

	/* Prototype_Component_Texture_Snow */
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_SNOW,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Snow/Snow.png"), 1); },
		TEXT("Prototype_Component_Texture_Snow"));

	// Shader


	// VIBuffer
	/* Prototype_Component_VIBuffer_Terrain */
	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_TERRAIN,
		[this] { return CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Height.bmp")); },
		TEXT("Prototype_Component_VIBuffer_Terrain"));

	/* Prototype_Component_Model_Fiona */
	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_FIONA,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/Fiona/Fiona.wmodel"); },
		TEXT("Prototype_Component_Model_Fiona"));

	/* Prototype_Component_Model_ForkLift */
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_FORKLIFT,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/ForkLift/ForkLift.wmodel"); },
		TEXT("Prototype_Component_Model_ForkLift"));

	/* Prototype_Component_VIBuffer_Instance_Snow */
	CVIBuffer_Rect_Instance::RECT_INSTANCE_DESC SnowDesc{};
	SnowDesc.iNumInstance = 6000;
	SnowDesc.vCenter = _float3(0.f, 0.f, 0.f);
	SnowDesc.vPosOffset = _float3(129.f, 0.3f, 129.f);
	SnowDesc.vSizeRange = _float2(0.2f, 0.5f);
	SnowDesc.vSpeedRange = _float2(1.f, 3.f);
	SnowDesc.vLifeRange = _float2(4.f, 8.f);
	SnowDesc.isLoop = true;

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_INST_SNOW,
		[this, SnowDesc]() mutable { return CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &SnowDesc); },
		TEXT("Prototype_Component_VIBuffer_Instance_Snow"));

	/* Prototype_Component_VIBuffer_Instance_Explosion */
	CVIBuffer_Point_Instance::POINT_INSTANCE_DESC ExplDesc{};
	ExplDesc.iNumInstance = 600;
	ExplDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ExplDesc.vPosOffset = _float3(0.3f, 0.3f, 0.3f);
	ExplDesc.vSizeRange = _float2(0.1f, 0.3f);
	ExplDesc.vSpeedRange = _float2(1.f, 3.f);
	ExplDesc.vLifeRange = _float2(0.5f, 1.f);
	ExplDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ExplDesc.isLoop = true;

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_INST_EXPLOSION,
		[this, ExplDesc]() mutable { return CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &ExplDesc); },
		TEXT("Prototype_Component_VIBuffer_Instance_Explosion"));

	// Object
	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_MONSTER,
		[this] { return CBattle_Pokemon::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Monster"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_TERRAIN,
		[this] { return CTerrain::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Terrain"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_FORKLIFT,
		[this] { return CForkLift::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_ForkLift"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_PLAYER,
		[this] { return CPlayer::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Player"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_BODY_PLAYER,
		[this] { return CBody_Player::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Body_Player"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_WEAPON,
		[this] { return CWeapon::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Weapon"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SKY,
		[this] { return CSky::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Sky"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SNOW,
		[this] { return CSnow::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Snow"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_EXPLOSION,
		[this] { return CExplosion::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Explosion"));
#pragma endregion

	if (FAILED(Ready_Resources_For_Battle()))
		return E_FAIL;

	if (FAILED(Ready_Resources_For_Capture()))
		return E_FAIL;

	if (FAILED(Ready_Resources_For_Effect()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Battle()
{
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_BTN_FIGHT,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battle/battle_command_battle_%02d.png"), 4); },
		TEXT("Prototype_Component_Texture_Battle_Button_Fight"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_BTN_POKE,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battle/battle_command_poke_%02d.png"), 4); },
		TEXT("Prototype_Component_Texture_Battle_Button_Poke"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_BTN_BAG,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battle/battle_command_bag_%02d.png"), 4); },
		TEXT("Prototype_Component_Texture_Battle_Button_Bag"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_PLAYERPLATE,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battle/battle_PlayerPlate.png"), 1); },
		TEXT("Prototype_Component_Texture_Battle_PlayerPlate"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_ENEMYPLATE,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battle/battle_EnemyPlate.png"), 1); },
		TEXT("Prototype_Component_Texture_Battle_EnemyPlate"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_BALLPLATE,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battle/battle_BallPlate.png"), 1); },
		TEXT("Prototype_Component_Texture_Battle_BallPlate"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_BALLICON,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battle/ball_icon_%02d.png"), 4); },
		TEXT("Prototype_Component_Texture_Battle_BallIcon"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_NUM,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battle/battle_Num_%02d.png"), 10); },
		TEXT("Prototype_Component_Texture_Battle_Number"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_NUM_SLASH,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battle/battle_Num_slash.png"), 1); },
		TEXT("Prototype_Component_Texture_Battle_Number_Slash"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_STATUS_GENDER,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battle/status_gender_%02d.png"), 2); },
		TEXT("Prototype_Component_Texture_Status_Gender"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_MSGBOX,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battleMsg/Battle_MsgBox.png"), 1); },
		TEXT("Prototype_Component_Texture_Battle_MsgBox"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_MSGICON,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battleMsg/Battle_MsgIcon.png"), 1); },
		TEXT("Prototype_Component_Texture_Battle_MsgIcon"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_MOVE,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battleMove/BattleMove_%02d.png"), 36); },
		TEXT("Prototype_Component_Texture_Battle_Move"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_MOVEICON,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battleMove/BattleMoveIcon_%02d.png"), 18); },
		TEXT("Prototype_Component_Texture_Battle_MoveIcon"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_BTL_MOVESHADOW,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_battleMove/BattleMove_Shadow.png"), 1); },
		TEXT("Prototype_Component_Texture_Battle_MoveShadow"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_BMAP_TOWN,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/LGPE_Map/battle_town/battle_town.wmodel"); },
		TEXT("Prototype_Component_Model_Town01"));

	CMapObject::MAPOBJECT_DESC tMapDesc{};
	tMapDesc.iModelLevelIndex = ETOUI(LEVEL::GAMEPLAY);
	tMapDesc.strModelTag = PROTO_COM_MODEL_BMAP_TOWN;
	tMapDesc.pRenderRule = CRenderRule_Manager::GetInstance()->Find_OrLoadMappingRule("../../Resources/LGPE_Map/battle_town/battle_town_mapping.json");
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_BMAP_TOWN,
		[this, tMapDesc] { return CMapObject::Create(m_pDevice, m_pContext, tMapDesc); },
		TEXT("Prototype_BattleMap_Town"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_BATTLE_TRAINER,
		[this] { return CBattle_Trainer::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Battle_Trainer"));

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Capture()
{
	// ---------- Texture ----------
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_GET_BUTTON,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_get/poke_get_button_%02d.png"), 3); },
		TEXT("Prototype_Component_Texture_Get_Button"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_GET_ICON,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_get/poke_get_icon.png"), 1); },
		TEXT("Prototype_Component_Texture_Get_Icon"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_GET_INFO,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_get/poke_get_Info.png"), 1); },
		TEXT("Prototype_Component_Texture_Get_Info"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_GET_INFO2,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_get/poke_get_Info2.png"), 1); },
		TEXT("Prototype_Component_Texture_Get_Info2"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_GET_LINE_FILL,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_get/poke_get_info_line_fill.png"), 1); },
		TEXT("Prototype_Component_Texture_Get_Line_Fill"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_GET_LINE_BACK,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_get/poke_get_info_line_back.png"), 1); },
		TEXT("Prototype_Component_Texture_Get_Line_Back"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_GET_LINE2_FILL,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_get/poke_get_info_line2_fill.png"), 1); },
		TEXT("Prototype_Component_Texture_Get_Line2_Fill"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_GET_LINE2_BACK,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_get/poke_get_info_line2_back.png"), 1); },
		TEXT("Prototype_Component_Texture_Get_Line2_Back"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_TEX_GET_TEXT_LV,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_get/poke_get_text_Lv.png"), 1); },
		TEXT("Prototype_Component_Texture_Get_Text_LV"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_TEX_GET_TEXT_NUM,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/poke_get/poke_get_itm_number_%02d.png"), 10); },
		TEXT("Prototype_Component_Texture_Get_Text_Number"));



	// ---------- VIBuffer & Model ----------
	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_MONSTER_BALL,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/ball/ball.wmodel"); },
		TEXT("Prototype_Component_Model_MonsterBall"));



	// ---------- Objects ----------
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_ACTOR_CAPTURE_TARGET,
		[this] { return CActor_CaptureTarget::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Actor_CaptureTarget"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_MONSTER_BALL,
		[this] { return CMonsterBall::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_MonsterBall"));
	
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_CAPTURE_RING,
		[this] { return CCaptureRing::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_CaptureRing"));

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Effect()
{
	struct EFFECT_TEXTURE_PROTO_DESC
	{
		WNameID strProtoTag;
		const _tchar* pTextureFilePath;
		const _tchar* pDebugName;
	};

	const EFFECT_TEXTURE_PROTO_DESC EffectTextureDescs[] =
	{
			{ PROTO_COM_TEX_EFT_BALL_ABSORB_0_LINE702_SML_O, TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_0_line702_sml_o.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Absorb_0_Line702_Sml_O") },
			{ PROTO_COM_TEX_EFT_BALL_ABSORB_0_MASK702_O, TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_0_mask702_o.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Absorb_0_Mask702_O") },
			{ PROTO_COM_TEX_EFT_BALL_ABSORB_1_CIRCLE004_SML_M, TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_1_circle004_sml_m.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Absorb_1_Circle004_Sml_M") },
			{ PROTO_COM_TEX_EFT_BALL_ABSORB_1_CIRCLE005_SML_M, TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_1_circle005_sml_m.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Absorb_1_Circle005_Sml_M") },
			{ PROTO_COM_TEX_EFT_BALL_ABSORB_2_FIRE003_M, TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_2_fire003_m.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Absorb_2_Fire003_M") },
			{ PROTO_COM_TEX_EFT_BALL_ABSORB_2_HIT011_M, TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_2_hit011_m.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Absorb_2_Hit011_M") },
			{ PROTO_COM_TEX_EFT_BALL_ABSORB_3_FLOW701_O, TEXT("../../Resources/Effects/Ball_absorb/Ball_absorb_3_flow701_o.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Absorb_3_Flow701_O") },

			{ PROTO_COM_TEX_EFT_BALL_HIT_0_MASK702_O, TEXT("../../Resources/Effects/Ball_hit/fxpt_0_mask702_o.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Hit_0_Mask702_O") },
			{ PROTO_COM_TEX_EFT_BALL_HIT_0_SMOKE002_M, TEXT("../../Resources/Effects/Ball_hit/fxpt_0_smoke002_m.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Hit_0_Smoke002_M") },
			{ PROTO_COM_TEX_EFT_BALL_HIT_0_SMOKE005_M, TEXT("../../Resources/Effects/Ball_hit/fxpt_0_smoke005_m.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Hit_0_Smoke005_M") },
			{ PROTO_COM_TEX_EFT_BALL_HIT_0_SMOKE203_A, TEXT("../../Resources/Effects/Ball_hit/fxpt_0_smoke203_a.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Hit_0_Smoke203_A") },
			{ PROTO_COM_TEX_EFT_BALL_HIT_0_SMOKE702_O, TEXT("../../Resources/Effects/Ball_hit/fxpt_0_smoke702_o.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Hit_0_Smoke702_O") },
			{ PROTO_COM_TEX_EFT_BALL_HIT_1_CIRCLE003_SML_M, TEXT("../../Resources/Effects/Ball_hit/fxpt_1_circle003_sml_m.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Hit_1_Circle003_Sml_M") },
			{ PROTO_COM_TEX_EFT_BALL_HIT_1_FLASH001_SML_M, TEXT("../../Resources/Effects/Ball_hit/fxpt_1_flash001_sml_m.png"), TEXT("Prototype_Component_Texture_Effect_Ball_Hit_1_Flash001_Sml_M") },

			{ PROTO_COM_TEX_EFT_CAPTURE_FAILED_0_BLUR003_SML_M, TEXT("../../Resources/Effects/Capture_failed/fxpt_0_blur003_sml_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Failed_0_Blur003_Sml_M") },
			{ PROTO_COM_TEX_EFT_CAPTURE_FAILED_0_LINE701_O, TEXT("../../Resources/Effects/Capture_failed/fxpt_0_line701_o.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Failed_0_Line701_O") },
			{ PROTO_COM_TEX_EFT_CAPTURE_FAILED_0_MASK702_O, TEXT("../../Resources/Effects/Capture_failed/fxpt_0_mask702_o.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Failed_0_Mask702_O") },
			{ PROTO_COM_TEX_EFT_CAPTURE_FAILED_0_SMOKE702_O, TEXT("../../Resources/Effects/Capture_failed/fxpt_0_smoke702_o.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Failed_0_Smoke702_O") },
			{ PROTO_COM_TEX_EFT_CAPTURE_FAILED_1_CIRCLE004_SML_M, TEXT("../../Resources/Effects/Capture_failed/fxpt_1_circle004_sml_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Failed_1_Circle004_Sml_M") },
			{ PROTO_COM_TEX_EFT_CAPTURE_FAILED_1_CIRCLE005_SML_M, TEXT("../../Resources/Effects/Capture_failed/fxpt_1_circle005_sml_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Failed_1_Circle005_Sml_M") },
			{ PROTO_COM_TEX_EFT_CAPTURE_FAILED_2_DUST701_O, TEXT("../../Resources/Effects/Capture_failed/fxpt_2_dust701_o.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Failed_2_Dust701_O") },
			{ PROTO_COM_TEX_EFT_CAPTURE_FAILED_2_HIT011_M, TEXT("../../Resources/Effects/Capture_failed/fxpt_2_hit011_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Failed_2_Hit011_M") },
			{ PROTO_COM_TEX_EFT_CAPTURE_FAILED_2_SMOKE703_O, TEXT("../../Resources/Effects/Capture_failed/fxpt_2_smoke703_o.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Failed_2_Smoke703_O") },
			{ PROTO_COM_TEX_EFT_CAPTURE_FAILED_2_WATER009_M, TEXT("../../Resources/Effects/Capture_failed/fxpt_2_water009_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Failed_2_Water009_M") },

			{ PROTO_COM_TEX_EFT_CAPTURE_HIT_0_LINE702_O, TEXT("../../Resources/Effects/Capture_hit/fxpt_0_line702_o.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Hit_0_Line702_O") },
			{ PROTO_COM_TEX_EFT_CAPTURE_HIT_0_MASK702_O, TEXT("../../Resources/Effects/Capture_hit/fxpt_0_mask702_o.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Hit_0_Mask702_O") },
			{ PROTO_COM_TEX_EFT_CAPTURE_HIT_0_MASK703_O, TEXT("../../Resources/Effects/Capture_hit/fxpt_0_mask703_o.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Hit_0_Mask703_O") },
			{ PROTO_COM_TEX_EFT_CAPTURE_HIT_0_SMOKE702_O, TEXT("../../Resources/Effects/Capture_hit/fxpt_0_smoke702_o.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Hit_0_Smoke702_O") },
			{ PROTO_COM_TEX_EFT_CAPTURE_HIT_1_CIRCLE005_M, TEXT("../../Resources/Effects/Capture_hit/fxpt_1_circle005_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Hit_1_Circle005_M") },
			{ PROTO_COM_TEX_EFT_CAPTURE_HIT_1_FLASH001_SML_M, TEXT("../../Resources/Effects/Capture_hit/fxpt_1_flash001_sml_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Hit_1_Flash001_Sml_M") },
			{ PROTO_COM_TEX_EFT_CAPTURE_HIT_2_DUST701_O, TEXT("../../Resources/Effects/Capture_hit/fxpt_2_dust701_o.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Hit_2_Dust701_O") },
			{ PROTO_COM_TEX_EFT_CAPTURE_HIT_2_SMOKE001_M, TEXT("../../Resources/Effects/Capture_hit/fxpt_2_smoke001_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Hit_2_Smoke001_M") },
			{ PROTO_COM_TEX_EFT_CAPTURE_HIT_2_TRANSFORM001_M, TEXT("../../Resources/Effects/Capture_hit/fxpt_2_transform001_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Hit_2_Transform001_M") },
			{ PROTO_COM_TEX_EFT_CAPTURE_HIT_3_CLOUD001_M, TEXT("../../Resources/Effects/Capture_hit/fxpt_3_cloud001_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Hit_3_Cloud001_M") },

			{ PROTO_COM_TEX_EFT_CAPTURE_SUCCESS_0_LINE701_O, TEXT("../../Resources/Effects/Capture_success/fxpt_0_line701_o.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Success_0_Line701_O") },
			{ PROTO_COM_TEX_EFT_CAPTURE_SUCCESS_0_MASK003_M, TEXT("../../Resources/Effects/Capture_success/fxpt_0_mask003_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Success_0_Mask003_M") },
			{ PROTO_COM_TEX_EFT_CAPTURE_SUCCESS_0_MA_STAR001_SML_M, TEXT("../../Resources/Effects/Capture_success/fxpt_0_ma_star001_sml_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Success_0_Ma_Star001_Sml_M") },
			{ PROTO_COM_TEX_EFT_CAPTURE_SUCCESS_1_CIRCLE001_SML_M, TEXT("../../Resources/Effects/Capture_success/fxpt_1_circle001_sml_m.png"), TEXT("Prototype_Component_Texture_Effect_Capture_Success_1_Circle001_Sml_M") },
	};

	for (const auto& Desc : EffectTextureDescs)
	{
		Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), Desc.strProtoTag,
			[this, pTextureFilePath = Desc.pTextureFilePath] { return CTexture::Create(m_pDevice, m_pContext, pTextureFilePath, 1); },
			Desc.pDebugName);
	}

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	if (!m_Threads.empty())
	{
		WaitForMultipleObjects(static_cast<DWORD>(m_Threads.size()), m_Threads.data(), TRUE, INFINITE);

		for (HANDLE h : m_Threads)
			if (h) CloseHandle(h);

		m_Threads.clear();
	}

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}