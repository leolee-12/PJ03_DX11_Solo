#include "..\Public\ContainerObject.h"
#include "GameInstance.h"
#include "PartObject.h"


CContainerObject::CContainerObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{
}

CContainerObject::CContainerObject(const CContainerObject & Prototype)
	: CGameObject{ Prototype }
{
}

CComponent * CContainerObject::Get_Component(_uint iPartID, const _wstring & strComponentTag)
{
	if (iPartID >= m_iNumParts)
		return nullptr;

	return m_Parts[iPartID]->Get_Component(strComponentTag);
}

HRESULT CContainerObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CContainerObject::Initialize(void * pArg)
{
	CONTAINEROBJECT_DESC*		pDesc = static_cast<CONTAINEROBJECT_DESC*>(pArg);
	m_iNumParts = pDesc->iNumParts;
	m_Parts.resize(m_iNumParts);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CContainerObject::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);

	for (auto& pPartObject : m_Parts)
	{
		if (nullptr != pPartObject)
			pPartObject->Camera_Update(fTimeDelta);
	}		
}

void CContainerObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	for (auto& pPartObject : m_Parts)
	{
		if (nullptr != pPartObject)
			pPartObject->Update(fTimeDelta);
	}
}

void CContainerObject::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& pPartObject : m_Parts)
	{
		if (nullptr != pPartObject)
			pPartObject->Late_Update(fTimeDelta);
	}
}

HRESULT CContainerObject::Render(_float fTimeDelta)
{
	__super::Render(fTimeDelta);

	return S_OK;
}



HRESULT CContainerObject::Add_PartObject(_uint iPartIndex, const _wstring & strPrototypeTag, void * pArg)
{
	CPartObject*		pPartObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(strPrototypeTag, pArg));
	if (nullptr == pPartObject)
		return E_FAIL;

	m_Parts[iPartIndex] = pPartObject;	

	return S_OK;
}

void CContainerObject::Free()
{
	__super::Free();

	for (auto& pPartObject : m_Parts)
		Safe_Release(pPartObject);

	m_Parts.clear();

}
