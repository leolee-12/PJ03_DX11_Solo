#include "Battle_Trainer.h"
#include "GameInstance.h"

CBattle_Trainer::CBattle_Trainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_strName = L"Battle_Trainer";
}

CBattle_Trainer::CBattle_Trainer(const CBattle_Trainer& Prototype)
	: CGameObject{ Prototype }
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
	m_strModelProtoTag = pDesc->strModelProtoTag;

	if (m_iSide >= g_kBattleSideCount)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(17, true);

	const _float3& vPos = BattleSlotPose::vTrainerPos[m_iSide];
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(vPos.x, vPos.y, vPos.z, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), BattleSlotPose::fYawTrainer[m_iSide]);

	return S_OK;
}

void CBattle_Trainer::Priority_Update(_float fTimeDelta)
{
}

void CBattle_Trainer::Update(_float fTimeDelta)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta);
}

void CBattle_Trainer::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CBattle_Trainer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_TexDiff", i, MATERIAL_TYPE::DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBattle_Trainer::Ready_Components()
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_VTXANIMMESH,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), m_strModelProtoTag,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattle_Trainer::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceWIT(m_pShaderCom, "g_WITMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(), sizeof(_float))))
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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
