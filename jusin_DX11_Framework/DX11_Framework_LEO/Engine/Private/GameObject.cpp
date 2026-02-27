#include "GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& Prototype)
	: m_pDevice{ Prototype.m_pDevice }
	, m_pContext{ Prototype.m_pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		auto	pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
		m_iFlag = pDesc->iFlag;	// 연습용 변수(의미X)
	}

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	
	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;
	
	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{

}

void CGameObject::Update(_float fTimeDelta)
{

}

void CGameObject::Late_Update(_float fTimeDelta)
{

}

HRESULT CGameObject::Render()
{
	return S_OK;
}


void CGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}