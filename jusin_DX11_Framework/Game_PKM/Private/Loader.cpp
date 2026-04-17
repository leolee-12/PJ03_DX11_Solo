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

	if (FAILED(pLoader->Loading()))
		return -1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr));

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT initSuccessed = CoInitializeEx(nullptr, 0);

	HRESULT	hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Ready_Resources_For_Logo();
		break;
	case LEVEL::GAMEPLAY:
		hr = Ready_Resources_For_GamePlay();
		break;
	}

	if (SUCCEEDED(initSuccessed))
		CoUninitialize();

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}


#ifdef _DEBUG

void CLoader::Show()
{
	SetWindowText(m_pGameInstance->Get_HWND(), m_szLoadingText);
}

#endif

HRESULT CLoader::Ready_Resources_For_Logo()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중"));
	/* Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_COM_TEXTURE_BACKGROUND,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중"));


	lstrcpy(m_szLoadingText, TEXT("정점, 인덱스 버퍼 로딩 중"));


	lstrcpy(m_szLoadingText, TEXT("객체원본 로딩 중"));
	/* Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::LOGO), PROTO_OBJ_BACKGROUND,
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlay()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중"));
	/* Prototype_Component_Texture_Terrain_Diff */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEXTURE_TERRAIN_DIFF,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEXTURE_TERRAIN_MASK,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_BRUSH */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEXTURE_TERRAIN_BRUSH,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEXTURE_SKY,
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중"));
	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXNORTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXMESH,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXANIMMESH,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXCUBE,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_Player_LGPE */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_PLAYER_LGPE,
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Player_LGPE.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("정점, 인덱스 버퍼 로딩 중"));
	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_VIBUFFER_TERRAIN,
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_VIBUFFER_CUBE,
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Model_Fiona */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_FIONA,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/Fiona/Fiona.wmodel"))))
		return E_FAIL;

	/* Prototype_Component_Model_ForkLift */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_FORKLIFT,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/ForkLift/ForkLift.wmodel"))))
		return E_FAIL;

	/* Prototype_Component_Model_PM0001_00 */
	//_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_PM0001_00,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/PM0001_00/pm0001_00.wmodel"))))
		return E_FAIL;

	/* Prototype_Component_Model_PM0004_00 */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_PM0004_00,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/PM0004_00/pm0004_00.wmodel"))))
		return E_FAIL;

	/* Prototype_Component_Model_PM0007_00 */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_PM0007_00,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/PM0007_00/pm0007_00.wmodel"))))
		return E_FAIL;

	/* Prototype_Component_Model_PM0025_00 */
 	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_PM0025_00,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/PM0025_00/pm0025_00.wmodel"))))
		return E_FAIL;

	/* Prototype_Component_Model_Hero */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_MODEL_HERO,
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/Hero/tr0001_00.wmodel"))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션 로딩 중"));
	/* Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_NAVIGATION,
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../DataFiles/Navigation.dat"), TEXT("../../DataFiles/Neighbors.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("콜라이더 로딩 중"));
	/* Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_AABB,
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::AABB))))
		return E_FAIL;

	/* Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_OBB,
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::OBB))))
		return E_FAIL;

	/* Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_COLLIDER_SPHERE,
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체원형 로딩 중"));
	/* Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_TERRAIN,
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_CAMERA_FREE,
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_MONSTER,
		CPokemon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_ForkLift */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_FORKLIFT,
		CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_PLAYER,
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_BODY_PLAYER,
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_WEAPON,
		CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_SKY,
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Player_LGPE */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_PLAYER_LGPE,
		CPlayer_LGPE::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_BODY_HERO,
		CBody_Hero::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;


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

	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CriticalSection);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}