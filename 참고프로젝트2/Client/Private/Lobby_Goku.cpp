#include "stdafx.h"
#include "..\Public\Lobby_Goku.h"

#include "RenderInstance.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Lobby.h"

#include "Lobby_Frieza.h"
#include "UI_Lobby_Text.h"

#include "BattleInterface.h"
#include "UI_Define.h"
#include "UI_Manager.h"

CLobby_Goku::CLobby_Goku(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CLobby_Goku::CLobby_Goku(const CLobby_Goku& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CLobby_Goku::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLobby_Goku::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC Desc{};
	Desc.fRotationPerSec = 1.f;
	Desc.fSpeedPerSec = 1.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
	m_pTransformCom->Set_State_Position(_float3(0.f, 0.f, -32.f));
	//아이들
	m_pModelCom->SetUp_Animation(1, true, 0.1f);

	return S_OK;
}

void CLobby_Goku::Camera_Update(_float fTimeDelta)
{

}

void CLobby_Goku::Update(_float fTimeDelta)
{
	// 입력 처리
	_float3 vTargetDir = { 0.f, 0.f, 0.f }; // 목표 방향
	_bool bInput = false;

	// 현재 키 상태 저장
	bool bCurrentKeyUp = m_pGameInstance->Key_Pressing(DIK_UP);
	bool bCurrentKeyDown = m_pGameInstance->Key_Pressing(DIK_DOWN);
	bool bCurrentKeyLeft = m_pGameInstance->Key_Pressing(DIK_LEFT);
	bool bCurrentKeyRight = m_pGameInstance->Key_Pressing(DIK_RIGHT);

	// 키 입력에 따른 목표 방향 설정
	if (bCurrentKeyUp)
	{
		vTargetDir.z += 1.f;
		bInput = true;
	}
	if (bCurrentKeyDown)
	{
		vTargetDir.z -= 1.f;
		bInput = true;
	}
	if (bCurrentKeyLeft)
	{
		vTargetDir.x -= 1.f;
		bInput = true;
	}
	if (bCurrentKeyRight)
	{
		vTargetDir.x += 1.f;
		bInput = true;
	}

	if (bInput)
	{
		m_pModelCom->SetUp_Animation(0, true, 0.1f);

		// 회전 처리
		RotateTowardsTarget(vTargetDir, fTimeDelta);

		// 이동 처리
		MoveForward(fTimeDelta);

		// 발소리 타이머 업데이트
		m_fFootstepTimer += fTimeDelta;
		if (m_fFootstepTimer >= m_fFootstepInterval)
		{
			m_pGameInstance->Play_Sound(CSound_Manager::SOUND_KEY_NAME::LOBBY_FOOT_SFX, false, 0.5f);
			m_fFootstepTimer = 0.f; // 타이머 리셋
		}
	}
	else
	{
		// 애니메이션 설정: 입력이 없을 때 기본 애니메이션 재생 (인덱스 1)
		m_pModelCom->SetUp_Animation(1, true, 0.1f);

		// 대쉬 애니메이션이 끝났음을 표시
		m_bDashTriggered = false;

		// 타이머 리셋
		m_fFootstepTimer = 0.f;
	}

	//걸었을 때 이펙트 생성
	CreateRunDustEffect(bInput ,0.25f ,fTimeDelta);

	// 현재 프레임의 애니메이션 재생
	m_pModelCom->Play_Animation(fTimeDelta * 3.f);

	// 이전 키 상태 업데이트
	m_bPrevKeyUp = bCurrentKeyUp;
	m_bPrevKeyDown = bCurrentKeyDown;
	m_bPrevKeyLeft = bCurrentKeyLeft;
	m_bPrevKeyRight = bCurrentKeyRight;

	// 레벨 이동
	
	Talk_Frieza(5.f);
	Talk_Staff(5.f);
	 
	Entry_Level();

}

void CLobby_Goku::Late_Update(_float fTimeDelta)
{
	m_pRenderInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLobby_Goku::Render(_float fTimeDelta)
{ 
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/* 모델이 가지고 있는 머테리얼 중 i번째 메시가 사용해야하는 머테리얼구조체의 aiTextureType_DIFFUSE번째 텍스쳐를 */
		/* m_pShaderCom에 있는 g_DiffuseTexture변수에 던져. */
		if (FAILED(m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;

		//m_pModelCom->Bind_MaterialSRV(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i);

		/* 모델이 가지고 있는 뼈들 중에서 현재 렌더링할려고 했던 i번째ㅑ 메시가 사용하는 뼈들을 배열로 만들어서 쉐이더로 던져준다.  */
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLobby_Goku::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_LOBBY, TEXT("Prototype_Component_Model_Lobby_Goku"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Goku::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

inline float Lobby_Goku_Clamp(float value, float minVal, float maxVal)
{
	if (value < minVal)
		return minVal;
	if (value > maxVal)
		return maxVal;
	return value;
}

void CLobby_Goku::RotateTowardsTarget(const _float3& vTargetDir, _float fTimeDelta)
{
	// 회전 속도 (라디안/초)
	const _float ROTATION_SPEED = XM_PI * 5.f; // 180도/초

	// 목표 방향 벡터를 로드하고 y를 0으로 설정하여 xz 평면에 투영
	XMVECTOR vTargetLook = XMLoadFloat3(&vTargetDir);
	vTargetLook = XMVectorSetY(vTargetLook, 0.f);
	vTargetLook = XMVector3Normalize(vTargetLook);

	// 현재 Look 벡터를 가져와서 y를 0으로 설정하여 xz 평면에 투영
	XMVECTOR vCurrentLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vCurrentLook = XMVectorSetY(vCurrentLook, 0.f);
	vCurrentLook = XMVector3Normalize(vCurrentLook);

	// 두 벡터 사이의 각도 계산
	float fDot = XMVectorGetX(XMVector3Dot(vCurrentLook, vTargetLook));
	fDot = Lobby_Goku_Clamp(fDot, -1.0f, 1.0f); // acos의 정의역을 벗어나지 않도록 클램핑
	float fAngle = acosf(fDot);

	// 회전 방향 결정 (외적의 y 성분 사용)
	XMVECTOR vCross = XMVector3Cross(vCurrentLook, vTargetLook);
	float fSign = (XMVectorGetY(vCross) < 0.0f) ? -1.0f : 1.0f;

	// 회전 각도 제한
	float fRotationAngle = min(fAngle, ROTATION_SPEED * fTimeDelta);

	// 회전 각도에 회전 방향 적용
	float fDeltaAngle = fRotationAngle * fSign;

	// y축 회전 행렬 생성
	XMMATRIX mRotation = XMMatrixRotationY(fDeltaAngle);

	// 현재 월드 행렬 가져오기
	XMMATRIX mWorld = m_pTransformCom->Get_WorldMatrix();

	// 회전 행렬을 현재 월드 행렬에 적용
	mWorld = mRotation * mWorld;

	_float4x4 matrix{};
	XMStoreFloat4x4(&matrix, mWorld);

	// 새로운 월드 행렬 설정
	m_pTransformCom->Set_WorldMatrix(matrix);
}

void CLobby_Goku::MoveForward(_float fTimeDelta)
{
	// 상수 정의
	const _float MOVE_SPEED = 15.f; // 이동 속도 (유닛/초)

	// 현재 Look 벡터를 가져와서 정규화
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	// 이동 거리 계산 (속도 * 시간)
	_float3 vMoveDistance;
	XMStoreFloat3(&vMoveDistance, vLook * fTimeDelta * MOVE_SPEED);

	// 현재 위치 가져오기
	_float3 vCurrentPos{};
	XMStoreFloat3(&vCurrentPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	// 새로운 위치 계산
	_float3 vNewPos = _float3(vCurrentPos.x + vMoveDistance.x, vCurrentPos.y + vMoveDistance.y, vCurrentPos.z + vMoveDistance.z);

	// 새로운 위치 설정
	m_pTransformCom->Set_State_Position(vNewPos);
}

void CLobby_Goku::Entry_Level()
{
	//_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_float x = XMVectorGetX(position);
	//_float z = XMVectorGetZ(position);

	_bool bTalkToGamePlay = dynamic_cast<CUI_Lobby_Text*>(m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextBox")))->Get_Finish();
	_bool bBuildingToGamePlay =  (10.f >= ObjectDistance(TEXT("Layer_Lobby_Arcade_Building"), 0));

	if (bTalkToGamePlay && m_bNextLevelText)
	{
		Set_CharacterInfo();
		m_bOnMessageBox[0] = FALSE;
		m_bOnMessageBox[1] = FALSE;
		m_bOnMessageBox[2] = FALSE;

		// m_eLevelID 
		CUI_Manager::Get_Instance()->m_eLevelID = LEVEL_GAMEPLAY;
		CLevel_Lobby* level_Lobby = static_cast<CLevel_Lobby*>( m_pGameInstance->Get_Level());
		level_Lobby->Change_Level();
	}

	if (bBuildingToGamePlay)
	{
		CUI_Manager::Get_Instance()->m_eLevelID = LEVEL_CHARACTER;
		CLevel_Lobby* level_Lobby = static_cast<CLevel_Lobby*>(m_pGameInstance->Get_Level());
		level_Lobby->Change_Level();
	}
}

void CLobby_Goku::Set_CharacterInfo()
{
	CBattleInterface_Manager::Get_Instance()->Set_CharaDesc(0, 1, CUI_Define::LPLAYER1, TEXT("Prototype_GameObject_Play_Goku"), CUI_Define::GOKU);
	CBattleInterface_Manager::Get_Instance()->Set_CharaDesc(1, 2, CUI_Define::RPLAYER1, TEXT("Prototype_GameObject_Play_Frieza"), CUI_Define::FRIEZA);
	CBattleInterface_Manager::Get_Instance()->Set_b1VS1(true);
}

_float CLobby_Goku::ObjectDistance(_wstring strLayerTag, _uint iLayerIndex)
{
	CTransform* pObjectTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_LOBBY, strLayerTag, TEXT("Com_Transform"), iLayerIndex));
	_vector vObjectPos = pObjectTransform->Get_State(CTransform::STATE_POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	return  GetVectorLength(vObjectPos - vMyPos);;
}

void CLobby_Goku::Talk_Frieza(_float fEnableDistance)
{
	_float fDistance = ObjectDistance(TEXT("Layer_Lobby_Frieza"));
	
	if (fEnableDistance >= fDistance)
	{
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_Key_Enter"))->SetActive(!m_bOnMessageBox[0]);

		if (m_bOnMessageBox[0] == FALSE && m_pGameInstance->Key_Down(DIK_RETURN))
		{
			CUI_Lobby_Text::UI_TEXT Desc = {};
			queue<CUI_Lobby_Text::UI_TEXT> Text;

			//Text.push({ TEXT("프리저! 네가 여기 나타난 순간부터 싸울 각오 했어! 이제 물러설 곳은 없어!"), CUI_Define::NPC_KRILLIN });
			//Text.push({ TEXT("하하하! 네가 감히 나에게 덤비다니, 웃음을 주는군. 하지만 오래가진 못할 거야."), CUI_Define::NPC_FRIEZA });
			//Text.push({ TEXT("내 힘을 무시하지 마라! 모두를 지키기 위해 난 싸운다!"), CUI_Define::NPC_KRILLIN });
			//Text.push({ TEXT("이것이 네 최선인가? 내 손끝 하나로 너를 끝내주마."), CUI_Define::NPC_FRIEZA });
			//Text.push({ TEXT("프리저, 네 상대는 나야! 크리링, 쉬고 있어. 이제 내가 나선다."), CUI_Define::NPC_GOKU });
			//Text.push({ TEXT("오공... 또 네가 끼어드는군. 이번엔 널 완전히 없애주겠다!"), CUI_Define::NPC_FRIEZA });


			Text.push({ TEXT("도망가라 크리링."), CUI_Define::NPC_GOKU });
			Text.push({ TEXT("빨리 가, 너희는 방해만 돼."), CUI_Define::NPC_GOKU });
			Text.push({ TEXT("네놈들을 용서할 것 같아?"), CUI_Define::NPC_FRIEZA });
			Text.push({ TEXT("한 마리도 남김없이 살려서는 못 보내"), CUI_Define::NPC_FRIEZA });
			Text.push({ TEXT("어...?"), CUI_Define::NPC_KRILLIN });
			Text.push({ TEXT("크리링!!!"), CUI_Define::NPC_GOKU});
			Text.push({ TEXT("그만둬 프리저!"), CUI_Define::NPC_GOKU });
			Text.push({ TEXT("오... 오공!!"), CUI_Define::NPC_KRILLIN });
			//Text.push({ TEXT("저기에서 무슨 이상한 소리가 들리지 않나,,?"), CUI_Define::ID_END})

			dynamic_cast<CUI_Lobby_Text*>(m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextBox")))->Add_Text(Text);

			m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextBox"))->SetActive(TRUE);
			m_bOnMessageBox[0] = TRUE;
			m_bNextLevelText = TRUE;
		}
	}
	else 
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_Key_Enter"))->SetActive(FALSE);
}

void CLobby_Goku::Talk_Staff(_float fEnableDistance)
{
	_float fDistance = ObjectDistance(TEXT("Layer_Lobby_Staff"), 0);
	_float fDistance2 = ObjectDistance(TEXT("Layer_Lobby_Staff"), 1);

	CUI_Lobby_Text::UI_TEXT Desc = {};
	queue<CUI_Lobby_Text::UI_TEXT> Text;

	if (fEnableDistance >= fDistance)
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_Key_Enter"), 1)->SetActive(!m_bOnMessageBox[1]);
	else 
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_Key_Enter"), 1)->SetActive(FALSE);


	if (fEnableDistance >= fDistance2)
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_Key_Enter"), 2)->SetActive(!m_bOnMessageBox[2]);
	else
		m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_Key_Enter"), 2)->SetActive(FALSE);


	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		if (fEnableDistance >= fDistance)
		{
			//m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_Key_Enter"),1)->SetActive(!m_bOnMessageBox[1]);
			Text.push({ TEXT("저기에서 무슨 이상한 소리가 들리지 않나,,?"), CUI_Define::ID_END });
			dynamic_cast<CUI_Lobby_Text*>(m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextBox")))->Add_Text(Text);
			m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextBox"))->SetActive(TRUE);
			m_bNextLevelText = FALSE;
			//m_bOnMessageBox[1] = TRUE;
		}

		if (fEnableDistance >= fDistance2)
		{
			//m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_Key_Enter"),2)->SetActive(!m_bOnMessageBox[2]);
			Text.push({ TEXT("무슨 문제가 발생한 거 같은데..."), CUI_Define::ID_END });
			dynamic_cast<CUI_Lobby_Text*>(m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextBox")))->Add_Text(Text);
			m_pGameInstance->Get_GameObject(LEVEL_LOBBY, TEXT("Layer_Lobby_TextBox"))->SetActive(TRUE);
			m_bNextLevelText = FALSE;
			//m_bOnMessageBox[2] = TRUE;
		}
	

		
	}
}

void CLobby_Goku::CreateRunDustEffect(_bool bOnInput ,_float fCreateDuration , _float fTimeDelta)
{
	CreateDustTimer += fTimeDelta;
	if (bOnInput && CreateDustTimer >= fCreateDuration)
	{
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_LOBBY, TEXT("Prototype_GameObject_Lobby_Goku_RunEff"), TEXT("Layer_Lobby_Goku_RunEff"));
		CreateDustTimer = 0.f;
	}
}

CLobby_Goku* CLobby_Goku::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLobby_Goku* pInstance = new CLobby_Goku(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLobby_Goku"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLobby_Goku::Clone(void* pArg)
{
	CLobby_Goku* pInstance = new CLobby_Goku(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLobby_Goku"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby_Goku::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
