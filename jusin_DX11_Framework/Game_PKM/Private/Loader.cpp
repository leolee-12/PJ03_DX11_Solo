#include "Loader.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Monster.h"
#include "Pokemon.h"
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

	CLoader::TaskFunc task;
	while (pTaskQueue->try_pop(task))
	{
		if (FAILED(task()))
			pLoader->Set_Error(true);

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
	_uint iWorkerCount = max(1u, min(iHW - 1u, iHW * 2 / 3));
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

#ifdef _DEBUG

void CLoader::Show()
{
	_wstring strLoadText =	to_wstring(m_iCompletedCount.load()) + L" / "
							+ to_wstring(m_iTotalCount) + L" ("
							+ to_wstring(m_Threads.size()) + L"개 스레드 가동 중)";

	SetWindowText(m_pGameInstance->Get_HWND(), strLoadText.c_str());
}

#endif

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
	auto Enqueue = [this](TaskFunc fn)
		{
			m_TaskQueue.push(move(fn));
			++m_iTotalCount;
		};
	
	// ---------- Texture ----------
	/* Prototype_Component_Texture_Title_pbgf_Diff */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEXTURE_TITLE_PBGF_DIFF,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/title/title_pbgf_00.png"), 1)); });

	/* Prototype_Component_Texture_Title_Logo_Diff */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEXTURE_TITLE_LOGO_DIFF,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/title/title_logo_%02d.png"), 3)); });

	/* Prototype_Component_Texture_Title_pbtn_Diff */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEXTURE_TITLE_PBTN_DIFF,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/title/title_pbtn_%02d.png"), 1)); });

	/* Prototype_Component_Texture_Title_Pika */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEXTURE_TITLE_PIKA,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/title/pber_pika_%02d.png"), 11)); });

	/* Prototype_Component_Texture_Star */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEXTURE_STAR,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Star/Star_%02d.png"), 3)); });

	// ---------- VIBuffer ----------
	/* Prototype_Component_VIBuffer_Instance_Star */
	CVIBuffer_UI_Instance::UI_INSTANCE_DESC StarDesc{};
	StarDesc.iNumInstance = 64;

	Enqueue([this, StarDesc]() mutable { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_VIBUFFER_INST_STAR,
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &StarDesc)); });

	/* Prototype_Component_Texture_BackGround */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEXTURE_BACKGROUND,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Default%d.jpg"), 2)); });

	// ---------- Shader ----------
	/* Prototype_Component_Shader_VtxUIInstance */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_SHADER_VTXUIINST,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxUIInstance.hlsl"), VTXUI_INSTANCE_DESC::Elements, VTXUI_INSTANCE_DESC::iNumElements)); });

	// ---------- GameObject ----------
	/* Prototype_GameObject_BackGround */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_OBJ_BACKGROUND,
		CBackGround::Create(m_pDevice, m_pContext)); });

	/* Prototype_GameObject_Effect_Star */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_OBJ_EFT_STAR,
		CEffect_Star::Create(m_pDevice, m_pContext)); });

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlay()
{
	auto Enqueue = [this](TaskFunc fn)
		{
			m_TaskQueue.push(move(fn));
			++m_iTotalCount;
		};

	// ---------- Texture ----------

	// ---------- Shader ----------
	/* Prototype_Component_Shader_VtxNorTex */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXNORTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements)); });

	/* Prototype_Component_Shader_VtxMesh */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXMESH,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements)); });

	/* Prototype_Component_Shader_VtxAnimMesh */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXANIMMESH,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)); });

	/* Prototype_Component_Shader_VtxCube */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXCUBE,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements)); });

	/* Prototype_Component_Shader_VtxRectInstance */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXRECTINST,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECT_INSTANCE_DESC::Elements, VTXRECT_INSTANCE_DESC::iNumElements)); });

	/* Prototype_Component_Shader_VtxPointInstance */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXPOINTINST,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINT_INSTANCE_DESC::Elements, VTXPOINT_INSTANCE_DESC::iNumElements)); });

	/* Prototype_Component_Shader_Player_LGPE */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_PLAYER_LGPE,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Player_LGPE.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)); });

	/* Prototype_Component_Shader_UI */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_UI,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_UI.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements)); });

	// ---------- VIBuffer ----------
	/* Prototype_Component_VIBuffer_Cube */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_VIBUFFER_CUBE,
		CVIBuffer_Cube::Create(m_pDevice, m_pContext)); });

	// ---------- Model ----------
	/* Prototype_Component_Model_PM0001_00 */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_PM0001_00,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/PM0001_00/pm0001_00.wmodel")); });

	/* Prototype_Component_Model_PM0004_00 */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_PM0004_00,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/PM0004_00/pm0004_00.wmodel")); });

	/* Prototype_Component_Model_PM0007_00 */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_PM0007_00,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/PM0007_00/pm0007_00.wmodel")); });

	/* Prototype_Component_Model_PM0025_00 */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_PM0025_00,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/PM0025_00/pm0025_00.wmodel")); });

	/* Prototype_Component_Model_Hero */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_HERO,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/Hero/tr0001_00.wmodel")); });

	/* Prototype_Component_Model_Town01 */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_TOWN01,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/LGPE_Map/area02/town01_2.wmodel")); });

	/* Prototype_Component_Model_Road01 */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_ROAD01,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/LGPE_Map/area02/road01.wmodel")); });

	// ---------- Navigation & Collider ----------
	/* Prototype_Component_Navigation_Map */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_NAVIGATION_MAP,
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../DataFiles/MapNaviMesh.nav"))); });

	/* Prototype_Component_Collider_AABB */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_AABB,
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::AABB)); });

	/* Prototype_Component_Collider_OBB */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_OBB,
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::OBB)); });

	/* Prototype_Component_Collider_Sphere */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_SPHERE,
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE)); });

	// ---------- Objects ----------
	/* Prototype_GameObject_Camera_Free */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_CAMERA_FREE,
		CCamera_Free::Create(m_pDevice, m_pContext)); });

	/* Prototype_GameObject_Player_LGPE */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_PLAYER_LGPE,
		CPlayer_LGPE::Create(m_pDevice, m_pContext)); });

	/* Prototype_GameObject_Body_Hero */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_BODY_HERO,
		CBody_Hero::Create(m_pDevice, m_pContext)); });

	/* Prototype_MapObject_Town01 */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_TOWN01,
		CMapObject::Create(m_pDevice, m_pContext, PROTO_COM_MODEL_TOWN01)); });

	/* Prototype_MapObject_Road01 */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_ROAD01,
		CMapObject::Create(m_pDevice, m_pContext, PROTO_COM_MODEL_ROAD01)); });

#pragma region STUDY
	// Texture
	/* Prototype_Component_Texture_Terrain_Diff */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEXTURE_TERRAIN_DIFF,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Tile%d.dds"), 2)); });

	/* Prototype_Component_Texture_Terrain_Mask */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEXTURE_TERRAIN_MASK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Mask.dds"), 1)); });

	/* Prototype_Component_Texture_Terrain_BRUSH */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEXTURE_TERRAIN_BRUSH,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Brush.png"), 1)); });

	/* Prototype_Component_Texture_Sky */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEXTURE_SKY,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/SkyBox/Sky_%d.dds"), 4)); });

	/* Prototype_Component_Texture_Snow */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEXTURE_SNOW,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Snow/Snow.png"), 1)); });

	// Shader


	// VIBuffer
	/* Prototype_Component_VIBuffer_Terrain */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_VIBUFFER_TERRAIN,
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Height.bmp"))); });

	/* Prototype_Component_Model_Fiona */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_FIONA,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/Fiona/Fiona.wmodel")); });

	/* Prototype_Component_Model_ForkLift */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_FORKLIFT,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/ForkLift/ForkLift.wmodel")); });

	/* Prototype_Component_VIBuffer_Instance_Snow */
	CVIBuffer_Rect_Instance::RECT_INSTANCE_DESC SnowDesc{};
	SnowDesc.iNumInstance = 6000;
	SnowDesc.vCenter = _float3(0.f, 0.f, 0.f);
	SnowDesc.vPosOffset = _float3(129.f, 0.3f, 129.f);
	SnowDesc.vSizeRange = _float2(0.2f, 0.5f);
	SnowDesc.vSpeedRange = _float2(1.f, 3.f);
	SnowDesc.vLifeRange = _float2(4.f, 8.f);
	SnowDesc.isLoop = true;

 	Enqueue([this, SnowDesc]() mutable { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_VIBUFFER_INST_SNOW,
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &SnowDesc)); });

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

	Enqueue([this, ExplDesc]() mutable { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_VIBUFFER_INST_EXPLOSION,
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &ExplDesc)); });

	// Navigation
	/* Prototype_Component_Navigation_Terrain */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_NAVIGATION_TERRAIN,
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../DataFiles/Navigation.dat"), TEXT("../../DataFiles/Neighbors.dat"))); });

	// Object
	/* Prototype_GameObject_Terrain */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_TERRAIN,
		CTerrain::Create(m_pDevice, m_pContext)); });

	/* Prototype_GameObject_Monster */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_MONSTER,
		CPokemon::Create(m_pDevice, m_pContext)); });

	/* Prototype_GameObject_ForkLift */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_FORKLIFT,
		CForkLift::Create(m_pDevice, m_pContext)); });

	/* Prototype_GameObject_Player */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_PLAYER,
		CPlayer::Create(m_pDevice, m_pContext)); });

	/* Prototype_GameObject_Body_Player */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_BODY_PLAYER,
		CBody_Player::Create(m_pDevice, m_pContext)); });

	/* Prototype_GameObject_Weapon */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_WEAPON,
		CWeapon::Create(m_pDevice, m_pContext)); });

	/* Prototype_GameObject_Sky */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SKY,
		CSky::Create(m_pDevice, m_pContext)); });

	/* Prototype_GameObject_Snow */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SNOW,
		CSnow::Create(m_pDevice, m_pContext)); });

	/* Prototype_GameObject_Explosion */
	Enqueue([this] { return m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_EXPLOSION,
		CExplosion::Create(m_pDevice, m_pContext)); });
#pragma endregion

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