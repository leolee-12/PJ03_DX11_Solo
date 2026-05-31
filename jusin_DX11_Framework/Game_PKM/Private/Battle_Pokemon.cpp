#include "Battle_Pokemon.h"
#include "Body_Pokemon.h"
#include "Body.h"
#include "PokemonData_Manager.h"
#include "RenderRule_Manager.h"
#include "Battle_Manager.h"
#include "Battle_AnimDef.h"
#include "Effect_Manager.h"

CBattle_Pokemon::CBattle_Pokemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
	m_strName = { L"Pokemon_Default" };
}

CBattle_Pokemon::CBattle_Pokemon(const CBattle_Pokemon& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CBattle_Pokemon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBattle_Pokemon::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const POKEMON_DESC* pDesc = static_cast<const POKEMON_DESC*>(pArg);

	if (nullptr == pDesc->pInstance)
		return E_FAIL;

	m_pInstance = pDesc->pInstance;
	m_iSide = pDesc->iSide;

	m_strBodyProtoTag = (0 != pDesc->strBodyProtoTag)
		? pDesc->strBodyProtoTag
		: PROTO_OBJ_BODY_POKEMON;

	m_strShaderProtoTag = (0 != pDesc->strShaderProtoTag)
		? pDesc->strShaderProtoTag
		: PROTO_COM_SHADER_POKEMON;
	m_iDefaultAnim = pDesc->iDefaultAnim;
	m_bLoop = pDesc->bLoop;
	m_fScale = pDesc->fScale;
	m_bBattleVisible = pDesc->bStartVisible;

	if (m_iSide >= g_kBattleSideCount)
		return E_FAIL;

	const CPokemonData_Manager* pDataMgr = CPokemonData_Manager::GetInstance();
	if (nullptr == pDataMgr)
		return E_FAIL;

	const SPECIES_DATA* pSpecies = pDataMgr->Find_Species(m_pInstance->iSpeciesID);
	if (nullptr == pSpecies || 0 == pSpecies->strModelTag)
		return E_FAIL;

	m_strSpeciesModelTag = pSpecies->strModelTag;
	if (0 == m_iDefaultAnim)
		m_iDefaultAnim = BattleAnim::Find_AnimIndex(m_strSpeciesModelTag, ANIM_KIND::IDLE);

	m_pRenderRule = pDesc->pRenderRule;
	if (nullptr == m_pRenderRule)
	{
		auto* pRuleManager = CRenderRule_Manager::GetInstance();
		if (nullptr == pRuleManager)
			return E_FAIL;

		m_pRenderRule = pRuleManager->Find_PokemonRenderRule(pSpecies);
	}

	if (nullptr == m_pRenderRule)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	const _float3& vPos = pDesc->vPos;
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(vPos.x, vPos.y, vPos.z, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), pDesc->fYaw);
	Reset_FaintDisappear();

	return S_OK;
}

void CBattle_Pokemon::Priority_Update(_float fTimeDelta)
{
}

void CBattle_Pokemon::Update(_float fTimeDelta)
{
	if (false == m_bBattleVisible)
		return;

	__super::Update(fTimeDelta);
	Tick_FaintDisappear(fTimeDelta);

	if (m_fAnimDuration > 0.f)
	{
		m_fAnimTimer += fTimeDelta;
		if (m_fAnimTimer >= m_fAnimDuration)
			Return_To_Idle();
	}
}

void CBattle_Pokemon::Late_Update(_float fTimeDelta)
{
	if (false == m_bBattleVisible)
		return;

	__super::Late_Update(fTimeDelta);
}

void CBattle_Pokemon::Tick_Movement(_float fTimeDelta)
{
}

void CBattle_Pokemon::Set_Manager(CBattle_Manager* pManager)
{
	m_pManager = pManager;
}

HRESULT CBattle_Pokemon::Apply_Switch(POKEMON_INSTANCE* pNewInstance)
{
	if (nullptr == pNewInstance)
		return E_FAIL;

	const CPokemonData_Manager* pDataMgr = CPokemonData_Manager::GetInstance();
	if (nullptr == pDataMgr)
		return E_FAIL;

	const SPECIES_DATA* pSpecies = pDataMgr->Find_Species(pNewInstance->iSpeciesID);
	if (nullptr == pSpecies || 0 == pSpecies->strModelTag)
		return E_FAIL;

	// SPokemonEnter (Begin_SendOutAppear) 가 visible=true 로 다시 켤 때까지 숨김
	m_bBattleVisible = false;
	Reset_FaintDisappear();

	auto ResetBattlePlacement = [this]()
		{
			if (nullptr == m_pManager || nullptr == m_pTransformCom)
				return;

			const _float3 vPos = m_pManager->Get_PokemonPos(m_iSide);
			m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(vPos.x, vPos.y, vPos.z, 1.f));
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pManager->Get_PokemonYaw(m_iSide));
		};

	if (m_bLockHeld && nullptr != m_pManager)
	{
		m_pManager->Release_Pacing_Lock();
		m_bLockHeld = false;
	}

	if (pSpecies->strModelTag == m_strSpeciesModelTag)
	{
		m_pInstance = pNewInstance;
		ResetBattlePlacement();

		if (nullptr != m_pBody)
			m_pBody->Set_Anim(BattleAnim::Find_AnimIndex(m_strSpeciesModelTag, ANIM_KIND::IDLE), true);

		m_eCurrentKind = ANIM_KIND::IDLE;
		m_fAnimTimer = 0.f;
		m_fAnimDuration = 0.f;
		return S_OK;
	}

	auto* pRuleManager = CRenderRule_Manager::GetInstance();
	if (nullptr == pRuleManager)
		return E_FAIL;

	const CRenderRule* pRenderRule = pRuleManager->Find_PokemonRenderRule(pSpecies);
	if (nullptr == pRenderRule)
		return E_FAIL;

	m_pBody = nullptr;

	if (FAILED(__super::Remove_PartObject(PART_BODY)))
		return E_FAIL;

	m_pInstance = pNewInstance;
	m_strSpeciesModelTag = pSpecies->strModelTag;
	m_pRenderRule = pRenderRule;
	m_iDefaultAnim = BattleAnim::Find_AnimIndex(m_strSpeciesModelTag, ANIM_KIND::IDLE);

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	ResetBattlePlacement();

	m_eCurrentKind = ANIM_KIND::IDLE;
	m_fAnimTimer = 0.f;
	m_fAnimDuration = 0.f;

	return S_OK;
}

void CBattle_Pokemon::Play_Attack(ANIM_KIND eAttackKind)
{
	Play_Anim_NonLoop(eAttackKind, 3.f);
}

void CBattle_Pokemon::Play_Hurt()
{
	Play_Anim_NonLoop(ANIM_KIND::HURT, 2.4f);
}

void CBattle_Pokemon::Play_Enter()
{
	if (nullptr != m_pBody)
		m_pBody->Set_Anim(
			BattleAnim::Find_AnimIndex(m_strSpeciesModelTag, ANIM_KIND::INTRO),
			false,
			0.f);

	m_eCurrentKind = ANIM_KIND::INTRO;
	m_fAnimTimer = 0.f;
	m_fAnimDuration = 2.f;
}

void CBattle_Pokemon::Play_Faint()
{
	// 기절은 IDLE 로 복귀하지 않음 - duration 0 으로 두고 model 만 변경 + 락 해제 (현재 단계에서 단순화)
	if (nullptr != m_pBody)
		m_pBody->Set_Anim(BattleAnim::Find_AnimIndex(m_strSpeciesModelTag, ANIM_KIND::FAINT),
			false);

	m_eCurrentKind = ANIM_KIND::FAINT;
	m_fAnimTimer = 0.f;
	m_fAnimDuration = 0.f;

	// 직전 Play_Hurt 등으로 잔존하는 락이 있으면 강제 해제 - KO 시점에서는 페이싱을 풀어야 다음단계 진행 가능
	if (m_bLockHeld && nullptr != m_pManager)
	{
		m_pManager->Release_Pacing_Lock();
		m_bLockHeld = false;
	}
}

void CBattle_Pokemon::Return_To_Idle()
{
	if (nullptr != m_pBody)
		m_pBody->Set_Anim(BattleAnim::Find_AnimIndex(m_strSpeciesModelTag, ANIM_KIND::IDLE), true);

	m_eCurrentKind = ANIM_KIND::IDLE;
	m_fAnimTimer = 0.f;
	m_fAnimDuration = 0.f;

	if (m_bLockHeld && nullptr != m_pManager)
	{
		m_pManager->Release_Pacing_Lock();
		m_bLockHeld = false;
	}
}

void CBattle_Pokemon::Begin_SendOutAppear()
{
	Reset_FaintDisappear();

	Play_Enter();

	if (nullptr != m_pBody)
	{
		m_pBody->Refresh_AnimationPose();
		m_pBody->Sync_CombinedWorldMatrix();   // visible 직전 포즈/결합행렬 동기화 - 큰 모델 첫 프레임 위치 튐 방지
	}

	CEffect_Manager* pEffectMgr = CEffect_Manager::GetInstance();
	if (nullptr != pEffectMgr)
	{
		pEffectMgr->PlayAt(
			"ball_absorb",
			Get_EffectPivot());
	}

	if (false == m_bBattleVisible)
		m_bBattleVisible = true;
}

void CBattle_Pokemon::Begin_FaintDisappear(_float fDuration)
{
	if (nullptr == m_pTransformCom)
		return;

	const _float3 vScale = m_pTransformCom->Get_Scaled();

	m_bBattleVisible = true;
	m_bFaintDisappear = true;
	m_fFaintDisappearTimer = 0.f;
	m_fFaintDisappearDuration = (fDuration > 0.001f) ? fDuration : 0.001f;
	m_fFaintDisappearStartScale = (vScale.x > 0.001f) ? vScale.x : 1.f;
}

_float3 CBattle_Pokemon::Get_EffectPivot() const
{
	return Get_SendOutEffectPos();
}

_float3 CBattle_Pokemon::Get_EffectPivot(EFFECT_SLOT eSlot, const _float3& vExtraOffset) const
{
	(void)eSlot;

	const _float3 vBase = Get_SendOutEffectPos();
	return _float3(
		vBase.x + vExtraOffset.x,
		vBase.y + vExtraOffset.y,
		vBase.z + vExtraOffset.z);
}

void CBattle_Pokemon::Play_Anim_NonLoop(ANIM_KIND eKind, _float fDuration)
{
	if (nullptr == m_pBody)
		return;

	const _uint iIndex = BattleAnim::Find_AnimIndex(m_strSpeciesModelTag, eKind);
	m_pBody->Set_Anim(iIndex, false);

	m_eCurrentKind = eKind;
	m_fAnimTimer = 0.f;
	m_fAnimDuration = fDuration;

	// 락 이미 보유 중인데 새 anim 으로 덮어쓰는 경우 - 기존 락 유지 (새로 잡지 않음).
	// 그러나 새 duration > 0 이어야 자동 복귀로 락이 풀린다. fDuration <= 0 으로 호출되는 경우는 Play_Faint 가 자체 처리.
	if (false == m_bLockHeld && nullptr != m_pManager && fDuration > 0.f)
	{
		m_pManager->Add_Pacing_Lock();
		m_bLockHeld = true;
	}
}

void CBattle_Pokemon::Reset_FaintDisappear()
{
	m_bFaintDisappear = false;
	m_fFaintDisappearTimer = 0.f;
	m_fFaintDisappearDuration = 0.45f;
	m_fFaintDisappearStartScale = 1.f;

	if (nullptr != m_pTransformCom)
		m_pTransformCom->ScaleTo(1.f, 1.f, 1.f);
}

void CBattle_Pokemon::Tick_FaintDisappear(_float fTimeDelta)
{
	if (false == m_bFaintDisappear || nullptr == m_pTransformCom)
		return;

	m_fFaintDisappearTimer += fTimeDelta;

	const _float fDuration =
		(m_fFaintDisappearDuration > 0.001f) ? m_fFaintDisappearDuration : 0.001f;

	_float fRatio = m_fFaintDisappearTimer / fDuration;
	if (fRatio > 1.f)
		fRatio = 1.f;

	const _float fSmooth = fRatio * fRatio * (3.f - 2.f * fRatio);
	_float fScale = m_fFaintDisappearStartScale * (1.f - fSmooth);
	if (fScale < 0.001f)
		fScale = 0.001f;

	m_pTransformCom->ScaleTo(fScale, fScale, fScale);

	if (fRatio >= 1.f)
	{
		m_bFaintDisappear = false;
		m_fFaintDisappearTimer = 0.f;
		m_fFaintDisappearStartScale = 1.f;
		m_bBattleVisible = false;
		m_pTransformCom->ScaleTo(1.f, 1.f, 1.f);
	}
}

HRESULT CBattle_Pokemon::Ready_PartObjects()
{
	CBody_Pokemon::BODY_POKEMON_DESC BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.strModelProtoTag = m_strSpeciesModelTag;
	BodyDesc.strShaderProtoTag = m_strShaderProtoTag;
	BodyDesc.pRenderRule = m_pRenderRule;
	BodyDesc.iDefaultAnim = m_iDefaultAnim;
	BodyDesc.bLoop = m_bLoop;
	BodyDesc.fScale = m_fScale;
	BodyDesc.bEnableRootMotion = false;
	BodyDesc.iRootMotionBoneIndex = 0;

	if (FAILED(__super::Add_PartObject(
		ETOUI(LEVEL::STATIC), m_strBodyProtoTag, PART_BODY, &BodyDesc)))
		return E_FAIL;

	m_pBody = Get_Part<CBody>(PART_BODY);

	return S_OK;
}

_float3 CBattle_Pokemon::Get_SendOutEffectPos() const
{
	_float3 vPos{};
	_vector vCenter = m_pTransformCom->Get_State(STATE::POSITION)
		+ XMVectorSet(0.f, 0.75f, 0.f, 0.f);

	XMStoreFloat3(&vPos, vCenter);
	return vPos;
}

CBattle_Pokemon* CBattle_Pokemon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBattle_Pokemon* pInstance = new CBattle_Pokemon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBattle_Pokemon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBattle_Pokemon::Clone(void* pArg)
{
	CBattle_Pokemon* pInstance = new CBattle_Pokemon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBattle_Pokemon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_Pokemon::Free()
{
	if (m_bLockHeld && nullptr != m_pManager)
	{
		m_pManager->Release_Pacing_Lock();
		m_bLockHeld = false;
	}

	m_pManager = nullptr;

	__super::Free();
}
