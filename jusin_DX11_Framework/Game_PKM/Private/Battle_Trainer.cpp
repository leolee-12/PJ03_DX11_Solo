#include "Battle_Trainer.h"
#include "Body_Human.h"
#include "RenderRule_Manager.h"

CBattle_Trainer::CBattle_Trainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
	m_strName = L"Battle_Trainer";
}

CBattle_Trainer::CBattle_Trainer(const CBattle_Trainer& Prototype)
	: CActor{ Prototype }
{
}

HRESULT CBattle_Trainer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBattle_Trainer::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	const BATTLE_TRAINER_DESC* pDesc = static_cast<const BATTLE_TRAINER_DESC*>(pArg);

	if (0 == pDesc->strModelProtoTag)
		return E_FAIL;

	m_iSide = pDesc->iSide;
	m_strBodyProtoTag = (0 != pDesc->strBodyProtoTag)
		? pDesc->strBodyProtoTag
		: PROTO_OBJ_BODY_HERO;

	m_strModelProtoTag = pDesc->strModelProtoTag;
	m_strModelTag = pDesc->strModelProtoTag;

	if (m_iSide >= g_kBattleSideCount)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects(pDesc)))
		return E_FAIL;

	const _float3& vPos = pDesc->vPos;
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(vPos.x, vPos.y, vPos.z, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), pDesc->fYaw);

	return S_OK;
}

void CBattle_Trainer::Priority_Update(_float fTimeDelta)
{
}

void CBattle_Trainer::Tick_Movement(_float fTimeDelta)
{
}

void CBattle_Trainer::Play_Intro()
{
	// INTRO는 loop가 아니라 마지막 자세에서 hold.
	// THROW가 시작되기 전까지 이 자세를 유지한다.
	Play_Anim_NonLoop(ANIM_KIND::INTRO, 0.f);
}

void CBattle_Trainer::Play_Throw()
{
	Play_Anim_NonLoop(ANIM_KIND::THROW, 0.8f);
}

void CBattle_Trainer::Play_Focus()
{
	Return_To_Focus();
}

void CBattle_Trainer::Play_Faint()
{
	Play_Anim_NonLoop(ANIM_KIND::FAINT, 0.f);
}

HRESULT CBattle_Trainer::Ready_PartObjects(const BATTLE_TRAINER_DESC* pDesc)
{
	if (PROTO_OBJ_BODY_HUMAN == m_strBodyProtoTag && '\0' != pDesc->szMappingPath[0])
	{
		CRenderRule_Manager* pRuleMgr = CRenderRule_Manager::GetInstance();
		if (nullptr == pRuleMgr)
			return E_FAIL;

		const CRenderRule* pRenderRule = pRuleMgr->Find_OrLoadMappingRule(pDesc->szMappingPath);
			if (nullptr == pRenderRule)
				return E_FAIL;

		CBody_Human::BODY_HUMAN_DESC BodyDesc{};
		BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
		BodyDesc.strModelProtoTag = pDesc->strModelProtoTag;
		BodyDesc.strShaderProtoTag = (0 != pDesc->strShaderProtoTag)
			? pDesc->strShaderProtoTag
			: PROTO_COM_SHADER_HUMAN;
		BodyDesc.iDefaultAnim = pDesc->iDefaultAnim;
		BodyDesc.bLoop = pDesc->bLoop;
		BodyDesc.fScale = pDesc->fScale;
		BodyDesc.bEnableRootMotion = false;
		BodyDesc.iRootMotionBoneIndex = 0;
		BodyDesc.pRenderRule = pRenderRule;

		if (FAILED(__super::Add_PartObject(
			ETOUI(LEVEL::STATIC), m_strBodyProtoTag, PART_BODY, &BodyDesc)))
			return E_FAIL;
	}
	else
	{
		CBody::BODY_DESC BodyDesc{};
		BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
		BodyDesc.strModelProtoTag = pDesc->strModelProtoTag;
		BodyDesc.strShaderProtoTag = (0 != pDesc->strShaderProtoTag)
			? pDesc->strShaderProtoTag
			: PROTO_COM_SHADER_VTXANIMMESH;
		BodyDesc.iDefaultAnim = pDesc->iDefaultAnim;
		BodyDesc.bLoop = pDesc->bLoop;
		BodyDesc.fScale = pDesc->fScale;
		BodyDesc.bEnableRootMotion = false;
		BodyDesc.iRootMotionBoneIndex = 0;

		if (FAILED(__super::Add_PartObject(
			ETOUI(LEVEL::STATIC), m_strBodyProtoTag, PART_BODY, &BodyDesc)))
			return E_FAIL;
	}

	m_pBody = Get_Part<CBody>(PART_BODY);
	if (nullptr == m_pBody)
		return E_FAIL;

	return S_OK;
}

void CBattle_Trainer::Play_Anim_NonLoop(ANIM_KIND eKind, _float fDuration)
{
	if (nullptr == m_pBody)
		return;

	const _uint iAnimIndex = BattleAnim::Find_AnimIndex(m_strModelTag, eKind);

	_float fBlendDuration = g_kDefaultBlendDuration;
	if (PROTO_COM_MODEL_PPL_WATER == m_strModelTag && ANIM_KIND::THROW == eKind)
		fBlendDuration = 0.f;

	m_pBody->Set_Anim(iAnimIndex, false, fBlendDuration);

	m_eCurrentKind = eKind;
	m_fAnimTimer = 0.f;
	m_fAnimDuration = (fDuration > 0.f) ? fDuration : 0.f;
}

void CBattle_Trainer::Play_Anim_Loop(ANIM_KIND eKind)
{
	if (nullptr == m_pBody)
		return;

	m_pBody->Set_Anim(BattleAnim::Find_AnimIndex(m_strModelTag, eKind), true);
	m_eCurrentKind = eKind;
	m_fAnimTimer = 0.f;
	m_fAnimDuration = 0.f;
}

void CBattle_Trainer::Return_To_Focus()
{
	if (nullptr == m_pBody)
		return;

	m_pBody->Set_Anim(BattleAnim::Find_AnimIndex(m_strModelTag, ANIM_KIND::FOCUS), true);
	m_eCurrentKind = ANIM_KIND::FOCUS;
	m_fAnimTimer = 0.f;
	m_fAnimDuration = 0.f;
}

CBattle_Trainer* CBattle_Trainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBattle_Trainer* pInstance = new CBattle_Trainer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBattle_Trainer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBattle_Trainer::Clone(void* pArg)
{
	CBattle_Trainer* pInstance = new CBattle_Trainer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBattle_Trainer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattle_Trainer::Free()
{
	__super::Free();
}