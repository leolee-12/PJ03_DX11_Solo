#include "Loader.h"

#include <chrono>
#include <sstream>

#include "UI_Defines.h"
#include "Effect_Defines.h"
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
#include "VIBuffer_Particle3D_Instance.h"
#include "VIBuffer_FieldGrass_Instance.h"
#include "VIBuffer_XZPlane.h"
#include "Effect_Star.h"
#include "Battle_Trainer.h"
#include "Battle_Ball.h"
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
#include "Interaction_DialogueBattle.h"
#include "Interaction_EventSequence.h"
#include "MonsterBall.h"
#include "CaptureRing.h"
#include "Effect_Test_Single.h"
#include "ParticleEmitter.h"
#include "Effect.h"
#include "Effect_Mesh.h"
#include "Effect_Manager.h"
#include "Camera_Director.h"
#include "WaterPlane.h"
#include "FieldGrass.h"
#include "FieldGrassBatch.h"
#include "Trail.h"
#include "VIBuffer_Trail.h"

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
			/* 로그 문자열은 로컬에서 조립 (race 없음) -> mutex 보호는 콘솔 쓰기에만 한정 */
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

	/* M7a 임시 코드 블록 (코드 기반 정의 등록) 전체 제거.
	   JSON 로드는 CEffect_Manager::Initialize() 내부에서 자동 수행. */
	CEffect_Manager::GetInstance()->Initialize();

	/* M1: Camera Director 싱글톤 초기화. 본 시점엔 빈 본문이나
   M9 에서 JSON 프리셋 로드를 본 함수 내부에서 처리. */
	CCamera_Director::GetInstance()->Initialize();

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
	if (FAILED(Ready_Resources_For_UI()))
		return E_FAIL;

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
	if (FAILED(Ready_Resources_For_UI()))
		return E_FAIL;

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_WATER,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/LGPE_Map/area02/sea%02d_com.png"), 5); },
			TEXT("Prototype_Component_Texture_Water_Net02"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_CLOUD,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/LGPE_Map/area02/cloud01.png"), 1); },
			TEXT("Prototype_Component_Texture_Cloud"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_FIELD_GRASS,
		[this] { return CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/grass/kusamura.png"), 1); },
		TEXT("Prototype_Component_Texture_FieldGrass"));
	
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

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_EFFECT_M1,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Effect_M1.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements); },
		TEXT("Prototype_Component_Shader_Effect_M1"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_PARTICLE3D,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Particle3D.hlsl"), VTXPARTICLE3D_INSTANCE_DESC::Elements,
			VTXPARTICLE3D_INSTANCE_DESC::iNumElements); },
		TEXT("Prototype_Component_Shader_Particle3D"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_EFFECT_BEAM,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Effect_Beam.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements); },
		TEXT("Prototype_Component_Shader_Effect_Beam"));

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

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_WATER,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Water.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements); },
		TEXT("Prototype_Component_Shader_Water"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_FIELD_GRASS,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_FieldGrass.hlsl"), VTXFIELDGRASS_INSTANCE_DESC::Elements, VTXFIELDGRASS_INSTANCE_DESC::iNumElements); },
			TEXT("Prototype_Component_Shader_FieldGrass"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_TRAIL,
		[this] { return CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Trail.hlsl"), VTXTRAIL_DESC::Elements, VTXTRAIL_DESC::iNumElements); },
		TEXT("Prototype_Component_Shader_Trail"));



	// ---------- VIBuffer ----------
	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_CUBE,
		[this] { return CVIBuffer_Cube::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_Component_VIBuffer_Cube"));

	/* Prototype_Component_VIBuffer_Instance_Effect - emitter용 (capacity 256) */
	CVIBuffer_Rect_Instance::RECT_INSTANCE_DESC EmitterVBDesc{};
	EmitterVBDesc.iNumInstance = 256;
	EmitterVBDesc.vCenter = _float3(0.f, 0.f, 0.f);
	EmitterVBDesc.vPosOffset = _float3(0.f, 0.f, 0.f);
	EmitterVBDesc.vSizeRange = _float2(0.2f, 0.5f);
	EmitterVBDesc.vSpeedRange = _float2(0.f, 0.f);   // 사용 안 함 (emitter가 직접 채움)
	EmitterVBDesc.vLifeRange = _float2(1.f, 1.f);   // 사용 안 함
	EmitterVBDesc.isLoop = false;

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_INST_EFFECT,
		[this, EmitterVBDesc]() mutable { return CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &EmitterVBDesc); },
		TEXT("Prototype_Component_VIBuffer_Instance_Effect"));

	/* Prototype_Component_VIBuffer_Instance_Particle3D — M4 신규 포맷 (capacity 256) */
	CVIBuffer_Particle3D_Instance::PARTICLE3D_INSTANCE_DESC Particle3DVBDesc{};
	Particle3DVBDesc.iNumInstance = 256;
	Particle3DVBDesc.vCenter = _float3(0.f, 0.f, 0.f);
	Particle3DVBDesc.vPosOffset = _float3(0.f, 0.f, 0.f);
	Particle3DVBDesc.vSizeRange = _float2(0.2f, 0.5f);

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_INST_PARTICLE3D,
		[this, Particle3DVBDesc]() mutable { return CVIBuffer_Particle3D_Instance::Create(m_pDevice, m_pContext, &Particle3DVBDesc); },
		TEXT("Prototype_Component_VIBuffer_Instance_Particle3D"));

	CVIBuffer_FieldGrass_Instance::FIELDGRASS_INSTANCE_DESC FieldGrassVBDesc{};
	FieldGrassVBDesc.iNumInstance = 1024;
	FieldGrassVBDesc.pModelFilePath = "../../Resources/Models/grass/grass.wmodel";

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_FIELD_GRASS_INST,
		[this, FieldGrassVBDesc]() mutable { return CVIBuffer_FieldGrass_Instance::Create(m_pDevice, m_pContext, &FieldGrassVBDesc); },
		TEXT("Prototype_Component_VIBuffer_FieldGrass_Instance"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_XZPLANE,
		[this] { return CVIBuffer_XZPlane::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_Component_VIBuffer_XZPlane"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_TRAIL,
		[this] { return CVIBuffer_Trail::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_Component_VIBuffer_Trail"));



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
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/people/hero/tr0001_00.wmodel"); },
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

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_HEROINE,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/people/heroine/heroine.wmodel"); },
			TEXT("Prototype_Component_Model_Heroine"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PPL_NURSE,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/people/nurse/nurse.wmodel"); },
			TEXT("Prototype_Component_Model_People_Nurse"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PPL_JUVENILES,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/people/juveniles/juveniles.wmodel"); },
			TEXT("Prototype_Component_Model_People_Juveniles"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_PPL_SHORTPANTS,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/people/shortpants/shortpants.wmodel"); },
			TEXT("Prototype_Component_Model_People_Shortpants"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_FIELD_GRASS,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/grass/grass.wmodel"); },
			TEXT("Prototype_Component_Model_FieldGrass"));

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

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_INTERACTION_DIALOGUE_BATTLE,
		[this] { return CInteraction_DialogueBattle::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_Component_Interaction_Dialogue_Battle"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_INTERACTION_EVENT_SEQUENCE,
		[this] { return CInteraction_EventSequence::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_Component_Interaction_EventSequence"));

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

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_EFFECT_TEST_SINGLE,
		[this] { return CEffect_Test_Single::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Effect_Test_Single"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_PARTICLE_EMITTER,
		[this] { return CParticleEmitter::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Particle_Emitter"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_EFFECT,
		[this] { return CEffect::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Effect"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_EFFECT_MESH,
		[this] { return CEffect_Mesh::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Effect_Mesh"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_TRAIL,
		[this] { return CTrail::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Trail"));

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

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_WATERPLANE,
		[this] { return CWaterPlane::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_WaterPlane"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_FIELD_GRASS,
		[this] { return CFieldGrass::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_FieldGrass"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_FIELD_GRASS_BATCH,
		[this] { return CFieldGrassBatch::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_FieldGrassBatch"));



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
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_BMAP_TOWN,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/LGPE_Map/battle_town/battle_town.wmodel"); },
		TEXT("Prototype_Component_Model_Town01"));

	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_BMAP_GRASS,
		[this] { return CModel::Create(m_pDevice, m_pContext, "../../Resources/LGPE_Map/battle/map_battle.wmodel"); },
		TEXT("Prototype_Component_Model_Grass"));

	CMapObject::MAPOBJECT_DESC tMapDesc{};
	tMapDesc.iModelLevelIndex = ETOUI(LEVEL::GAMEPLAY);
	tMapDesc.strModelTag = PROTO_COM_MODEL_BMAP_TOWN;
	tMapDesc.pRenderRule = CRenderRule_Manager::GetInstance()->Find_OrLoadMappingRule("../../Resources/LGPE_Map/battle_town/battle_town_mapping.json");
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_BMAP_TOWN,
		[this, tMapDesc] { return CMapObject::Create(m_pDevice, m_pContext, tMapDesc); },
		TEXT("Prototype_BattleMap_Town"));

	tMapDesc.iModelLevelIndex = ETOUI(LEVEL::GAMEPLAY);
	tMapDesc.strModelTag = PROTO_COM_MODEL_BMAP_GRASS;
	tMapDesc.pRenderRule = CRenderRule_Manager::GetInstance()->Find_OrLoadMappingRule("../../Resources/LGPE_Map/battle/map_battle_mapping.json");
	Enqueue_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_BMAP_GRASS,
		[this, tMapDesc] { return CMapObject::Create(m_pDevice, m_pContext, tMapDesc); },
		TEXT("Prototype_BattleMap_Grass"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_BATTLE_TRAINER,
		[this] { return CBattle_Trainer::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Battle_Trainer"));

	Enqueue_Prototype(ETOUI(LEVEL::STATIC), PROTO_OBJ_BATTLE_BALL,
		[this] { return CBattle_Ball::Create(m_pDevice, m_pContext); },
		TEXT("Prototype_GameObject_Battle_Ball"));

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Capture()
{
	// ---------- Texture ----------

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

HRESULT CLoader::Ready_Resources_For_UI()
{
	for (const auto& Desc : Game_PKM::g_UITextureOptions)
	{
		if (nullptr == Desc.pTextureFilePath)
			continue;

		Enqueue_Prototype(ETOUI(Desc.eLevel), Desc.strTag,
			[this,
			pTextureFilePath = Desc.pTextureFilePath,
			iNumTextures = Desc.iNumTextures]
			{
				return CTexture::Create(m_pDevice, m_pContext,
					pTextureFilePath, iNumTextures);
			},
			Desc.pDebugName);
	}

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

	for (const auto& Desc : Game_PKM::g_EffectTextureOptions)
	{
		if (nullptr == Desc.pTextureFilePath)
			continue;

		Enqueue_Prototype(ETOUI(LEVEL::STATIC), Desc.strTag,
			[this, pTextureFilePath = Desc.pTextureFilePath]
			{
				return CTexture::Create(m_pDevice, m_pContext, pTextureFilePath, 1);
			},
			Desc.pDebugName);
	}

	for (const auto& Desc : Game_PKM::g_EffectMeshOptions)
	{
		if (nullptr == Desc.pModelFilePath)
			continue;

		Enqueue_Prototype(ETOUI(LEVEL::STATIC), Desc.strTag,
			[this, pModelFilePath = Desc.pModelFilePath]
			{
				return CModel::Create(m_pDevice, m_pContext, pModelFilePath);
			},
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