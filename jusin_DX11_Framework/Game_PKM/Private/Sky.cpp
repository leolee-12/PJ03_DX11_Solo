#include "Sky.h"
#include "GameInstance.h"

CSky::CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_strName = { L"Sky" };
}

CSky::CSky(const CSky& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CSky::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSky::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CSky::Priority_Update(_float fTimeDelta)
{

}

void CSky::Update(_float fTimeDelta)
{

}

void CSky::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(STATE::POSITION,
		XMLoadFloat4(m_pGameInstance->Get_CamPosition()));
	m_pGameInstance->Add_RenderGroup(RENDERID::PRIORITY, this);
}

HRESULT CSky::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSky::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_SHADER_VTXCUBE,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_VIBUFFER_CUBE,
		COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_SKY,
		COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CSky::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
		return E_FAIL;

	return S_OK;
}


CSky* CSky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSky* pInstance = new CSky(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSky::Clone(void* pArg)
{
	CSky* pInstance = new CSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
