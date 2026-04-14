#include "Player_LGPE.h"
#include "GameInstance.h"

#include "Body_Hero.h"

CPlayer_LGPE::CPlayer_LGPE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
	m_strName = { L"Player_LGPE" };
}

CPlayer_LGPE::CPlayer_LGPE(const CPlayer_LGPE& Prototype)
	: CContainerObject{ Prototype }
{

}

HRESULT CPlayer_LGPE::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_LGPE::Initialize(void* pArg)
{
	GAMEOBJECT_DESC Desc{};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.f);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CPlayer_LGPE::Priority_Update(_float fTimeDelta)
{
	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Priority_Update(fTimeDelta);
		});
}

void CPlayer_LGPE::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_UP))
	{
		static_cast<CBody_Hero*>(m_PartObjects[PART_BODY])->Set_Anim(RUN, true);
	}
	else
		static_cast<CBody_Hero*>(m_PartObjects[PART_BODY])->Set_Anim(IDLE, true);

	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Update(fTimeDelta);
		});

	_vector vDelta = XMLoadFloat3(&static_cast<CBody_Hero*>(m_PartObjects[PART_BODY])->Get_RootMotionDelta());
	_vector vWorldDelta = XMVector3TransformNormal(vDelta, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	_vector vCurrPos = m_pTransformCom->Get_State(STATE::POSITION);
	m_pTransformCom->Set_State(STATE::POSITION, vCurrPos + vWorldDelta);
}

void CPlayer_LGPE::Late_Update(_float fTimeDelta)
{
	m_PartObjects.for_each([&fTimeDelta](auto& Pair)
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Update(fTimeDelta);
		});
}

HRESULT CPlayer_LGPE::Render()
{

	return S_OK;
}

HRESULT CPlayer_LGPE::Ready_Components()
{
	return S_OK;
}

HRESULT CPlayer_LGPE::Ready_PartObjects()
{
	CBody_Hero::BODY_HERO_DESC BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.pParentState = &m_iState;

	if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), PROTO_OBJ_BODY_HERO, PART_BODY, &BodyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_LGPE::Bind_ShaderResources()
{



	return S_OK;
}


CPlayer_LGPE* CPlayer_LGPE::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_LGPE* pInstance = new CPlayer_LGPE(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_LGPE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_LGPE::Clone(void* pArg)
{
	CPlayer_LGPE* pInstance = new CPlayer_LGPE(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_LGPE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_LGPE::Free()
{
	__super::Free();



}