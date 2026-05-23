#include "Body_Human.h"
#include "Battle_AnimDef.h"

#include "GameInstance.h"
#include "Model.h"

CBody_Human::CBody_Human(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBody{ pDevice, pContext }
{
	m_strName = L"Body_Human";
}

CBody_Human::CBody_Human(const CBody_Human& Prototype)
	: CBody{ Prototype }
{
}

HRESULT CBody_Human::Initialize(void* pArg)
{
	const BODY_HUMAN_DESC* pDesc = static_cast<const BODY_HUMAN_DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	BODY_HUMAN_DESC Desc = *pDesc;

	if (0 == Desc.strShaderProtoTag)
		Desc.strShaderProtoTag = PROTO_COM_SHADER_HUMAN;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(m_RenderProfile.Build(m_pModelCom, Desc.pRenderRule)))
		return E_FAIL;

	return S_OK;
}

void CBody_Human::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_1))
	{
		m_iCurrAnim++;
		if (m_iCurrAnim >= m_pModelCom->Get_NumAnimations()) m_iCurrAnim = 0;
		m_pModelCom->Set_AnimationIndex(m_iCurrAnim, false, 0.2f);
	}

	__super::Update(fTimeDelta);
}

HRESULT CBody_Human::Render()
{
	if (FAILED(Bind_ShaderResources_Common()))
		return E_FAIL;

	vector<CRenderProfile::MATERIAL_SLOT> Slots =
	{
	  { MATERIAL_TYPE::DIFFUSE, "g_TexDiff" },
	  { MATERIAL_TYPE::SPECULAR, "g_TexSpec" },
	  { MATERIAL_TYPE::AMBIENT, "g_TexAmbt" },
	};

	if (FAILED(m_RenderProfile.Bind_AndDraw(m_pShaderCom, Slots, "g_BoneMatrices")))
		return E_FAIL;

	return S_OK;
}

_matrix CBody_Human::Resolve_ShaderWorldMatrix() const
{
	_matrix WorldMatrix = __super::Resolve_ShaderWorldMatrix();

	if (nullptr == m_pModelCom)
		return WorldMatrix;

	const _uint iCurrAnimIndex = m_pModelCom->Get_CurrAnimIndex();
	const _matrix CorrectionMatrix =
		BattleAnim::Find_AnimRotationCorrection(m_strModelProtoTag, iCurrAnimIndex);

	return CorrectionMatrix * WorldMatrix;
}

CBody_Human* CBody_Human::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Human* pInstance = new CBody_Human(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody_Human");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Human::Clone(void* pArg)
{
	CBody_Human* pInstance = new CBody_Human(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Human");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Human::Free()
{
	__super::Free();
}