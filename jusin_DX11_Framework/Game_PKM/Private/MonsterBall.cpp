#include "MonsterBall.h"
#include "GameInstance.h"

CMonsterBall::CMonsterBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
	m_strName = L"MonsterBall";
}

CMonsterBall::CMonsterBall(const CMonsterBall& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CMonsterBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* DESC 가 주어지면 파라미터 override. nullptr 이면 헤더 디폴트 유지. */
	if (nullptr != pArg)
	{
		const MONSTER_BALL_DESC* pDesc = static_cast<const MONSTER_BALL_DESC*>(pArg);
		m_vStartPos = pDesc->vSpawnPos;
		m_vTargetPos = pDesc->vTargetPos;
		m_fFlightDuration = pDesc->fFlightDuration;
		m_fArcHeight = pDesc->fArcHeight;
		m_fImpactDuration = pDesc->fImpactDuration;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 시각 보정 — Scaling 후 Rotation(X축 -60°). CTransform::Rotation 은 현재 스케일을 보존하므로
	   Scaling → Rotation 순서로 호출 안전. Set_State(POSITION) 은 RIGHT/UP/LOOK 보존 → 회전/스케일
유지. */
	m_pTransformCom->Scaling(0.05f, 0.05f, 0.05f);
	m_pTransformCom->Rotation(XMConvertToRadians(-60.f), 0.f, 0.f);

	/* READY — 시작 위치에 정지. Launch 호출 전까지 이동 없음. */
	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(m_vStartPos.x, m_vStartPos.y, m_vStartPos.z, 1.f));

	/* 애니메이션 0번 루프 재생 — ball.wmodel 내부 정의 사용.
	   현재 셰이더는 VTXMESH(정적) — 본 변형의 시각 반영은 셰이더 VTXANIMMESH 교체 단위 후. */
	if (nullptr != m_pModelCom && m_pModelCom->Get_NumAnimations() > 0)
		m_pModelCom->Set_AnimationIndex(0, true);

	m_eState = BALL_STATE::READY;
	m_fElapsed = 0.f;

	return S_OK;
}

void CMonsterBall::Priority_Update(_float fTimeDelta)
{
}

void CMonsterBall::Update(_float fTimeDelta)
{
	/* 애니메이션 진행 — 본 단계에선 셰이더가 정적이라 시각 효과 미반영.
   Play_Animation 자체는 본 매트릭스 갱신을 수행 — 셰이더 교체 단위 후 가시화. */
	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta);

	switch (m_eState)
	{
	case BALL_STATE::READY:
		/* Launch() 호출 대기 — 이동/시간 진행 없음. */
		break;

	case BALL_STATE::FLYING:
		m_fElapsed += fTimeDelta;
		if (m_fElapsed >= m_fFlightDuration)
		{
			/* 도착점 정확 보정 후 IMPACT 전이. */
			m_fElapsed = m_fFlightDuration;
			Update_Position();
			m_eState = BALL_STATE::IMPACT;
			m_fElapsed = 0.f;
			OutputDebugStringW(L"[MonsterBall] FLYING → IMPACT\n");
		}
		else
		{
			Update_Position();
		}
		break;

	case BALL_STATE::IMPACT:
		m_fElapsed += fTimeDelta;
		if (m_fElapsed >= m_fImpactDuration)
		{
			/* DONE 전이만. 액터 소멸은 외부(레벨/매니저)가 책임. */
			m_eState = BALL_STATE::DONE;
			m_fElapsed = 0.f;
			OutputDebugStringW(L"[MonsterBall] IMPACT → DONE\n");
		}
		break;

	case BALL_STATE::DONE:
	default:
		break;
	}
}

void CMonsterBall::Late_Update(_float fTimeDelta)
{
	if (!m_bVisible)
		return;

	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CMonsterBall::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexDiff", i, MATERIAL_TYPE::DIFFUSE,
			0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CMonsterBall::Launch()
{
	/* READY 외 상태에서 들어온 Launch 는 무시. */
	if (BALL_STATE::READY != m_eState)
		return;

	m_eState = BALL_STATE::FLYING;
	m_fElapsed = 0.f;
	m_bVisible = true;   // 발사 시 무조건 가시화 (이전 Hide 가 있어도 복귀)
	OutputDebugStringW(L"[MonsterBall] READY → FLYING (Show)\n");
}

void CMonsterBall::Reset()
{
	/* READY 외 상태에서 들어온 Launch 는 무시. */
	if (BALL_STATE::READY != m_eState)
		return;

	m_eState = BALL_STATE::FLYING;
	m_fElapsed = 0.f;
	m_bVisible = true;   // 발사 시 무조건 가시화 (이전 Hide 가 있어도 복귀)
	OutputDebugStringW(L"[MonsterBall] READY → FLYING (Show)\n");
}

void CMonsterBall::Hide()
{
	m_bVisible = false;
	OutputDebugStringW(L"[MonsterBall] Hide\n");
}

void CMonsterBall::Show()
{
	m_bVisible = true;
	OutputDebugStringW(L"[MonsterBall] Show\n");
}

HRESULT CMonsterBall::Ready_Components()
{
	/* Shader — 정적 메시용 VTXMESH 재사용 (ForkLift/Weapon 패턴). */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXMESH,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Model — ball.wmodel (Loader 등록). */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_MODEL_MONSTER_BALL,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterBall::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceWIT(m_pShaderCom, "g_WITMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

void CMonsterBall::Update_Position()
{
	/* 0 ≤ t ≤ 1 정규화. fFlightDuration 0 보호. */
	const _float t = (m_fFlightDuration > 1e-6f)
		? min(m_fElapsed / m_fFlightDuration, 1.f)
		: 1.f;

	/* XZ 선형 보간, Y 선형 + 표준 포물선 가산.
	   arc 항 4·H·t·(1-t) 는 t=0/1 에서 0, t=0.5 에서 H — 정점 높이 = H. */
	const _float x = m_vStartPos.x + (m_vTargetPos.x - m_vStartPos.x) * t;
	const _float z = m_vStartPos.z + (m_vTargetPos.z - m_vStartPos.z) * t;
	const _float yLin = m_vStartPos.y + (m_vTargetPos.y - m_vStartPos.y) * t;
	const _float yArc = 4.f * m_fArcHeight * t * (1.f - t);
	const _float y = yLin + yArc;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(x, y, z, 1.f));
}

CMonsterBall* CMonsterBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterBall* pInstance = new CMonsterBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonsterBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterBall::Clone(void* pArg)
{
	CMonsterBall* pInstance = new CMonsterBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterBall::Free()
{
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

	__super::Free();
}