#include "..\Public\PartObject.h"
#include "Shader.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CPartObject::CPartObject(const CPartObject& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
	PARTOBJECT_DESC* pDesc = static_cast<PARTOBJECT_DESC*>(pArg);
	m_pParentMatrix = pDesc->pParentMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPartObject::Camera_Update(_float fTimeDelta)
{
	__super::Camera_Update(fTimeDelta);
}

void CPartObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CPartObject::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
}

HRESULT CPartObject::Render(_float fTimeDelta)
{
	__super::Render(fTimeDelta);

	return S_OK;
}

HRESULT CPartObject::Bind_WorldMatrix(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

void CPartObject::Free()
{
	__super::Free();


}
