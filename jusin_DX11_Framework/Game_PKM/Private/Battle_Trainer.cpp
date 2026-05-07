#include "Battle_Trainer.h"
#include "Body.h"

CBattle_Trainer::CBattle_Trainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
	m_strName = L"Battle_Trainer";
}

CBattle_Trainer::CBattle_Trainer(const CBattle_Trainer& Prototype)
	: CContainerObject{ Prototype }
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

void CBattle_Trainer::Update(_float fTimeDelta)
{
	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Update(fTimeDelta);
		});
}

void CBattle_Trainer::Late_Update(_float fTimeDelta)
{
	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Update(fTimeDelta);
		});
}

HRESULT CBattle_Trainer::Render()
{
	return S_OK;
}

HRESULT CBattle_Trainer::Ready_PartObjects(const BATTLE_TRAINER_DESC* pDesc)
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

	return S_OK;
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