#include "FieldGrassBatch.h"

#include "GameInstance.h"
#include "VIBuffer_FieldGrass_Instance.h"

namespace
{
	static constexpr _float FIELD_GRASS_SCALE = 0.3f;

	_uint Mix_GrassHash(_uint iHash, _uint iValue)
	{
		iValue ^= iValue >> 16;
		iValue *= 0x7feb352du;
		iValue ^= iValue >> 15;
		iValue *= 0x846ca68bu;
		iValue ^= iValue >> 16;

		return iHash ^ iValue;
	}

	_uint Float_ToHash(_float fValue)
	{
		return static_cast<_uint>(static_cast<_int>((fValue + 4096.f) * 1000.f));
	}

	_float Resolve_GrassYaw(_uint iIndex, const _float3& vPosition)
	{
		_uint iHash = 2166136261u;
		iHash = Mix_GrassHash(iHash, iIndex);
		iHash = Mix_GrassHash(iHash, Float_ToHash(vPosition.x));
		iHash = Mix_GrassHash(iHash, Float_ToHash(vPosition.y));
		iHash = Mix_GrassHash(iHash, Float_ToHash(vPosition.z));

		const _float fRatio = static_cast<_float>(iHash & 0xffffu) / 65535.f;
		return fRatio * XM_2PI;
	}
}

CFieldGrassBatch::CFieldGrassBatch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_strName = L"FieldGrassBatch";
}

CFieldGrassBatch::CFieldGrassBatch(const CFieldGrassBatch& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CFieldGrassBatch::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFieldGrassBatch::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	FIELDGRASS_BATCH_DESC Desc = *static_cast<FIELDGRASS_BATCH_DESC*>(pArg);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (0 < Desc.iNumPositions && nullptr == Desc.pPositions)
		return E_FAIL;

	if (Desc.iNumPositions > m_pVIBufferCom->Get_MaxInstances())
		return E_FAIL;

	if (FAILED(Build_Instances(Desc)))
		return E_FAIL;

	const VTXFIELDGRASS_INSTANCE* pInstances =
		m_Instances.empty() ? nullptr : m_Instances.data();

	if (FAILED(m_pVIBufferCom->Update_FieldGrass_Instances(
		pInstances, static_cast<_uint>(m_Instances.size()))))
		return E_FAIL;

	return S_OK;
}

void CFieldGrassBatch::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CFieldGrassBatch::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	return m_pVIBufferCom->Render();
}

HRESULT CFieldGrassBatch::Ready_Components()
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_FIELD_GRASS,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), PROTO_COM_TEX_FIELD_GRASS,
		COM_TEXTURE, reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_VIBUFFER_FIELD_GRASS_INST,
		COM_VIBUFFER, reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFieldGrassBatch::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResourceWIT(m_pShaderCom, "g_WITMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		m_pGameInstance->Get_Transform(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		m_pGameInstance->Get_Transform(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZPtr(),
		sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TexDiff", 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFieldGrassBatch::Build_Instances(const FIELDGRASS_BATCH_DESC& Desc)
{
	m_Instances.clear();

	if (0 == Desc.iNumPositions)
		return S_OK;

	m_Instances.reserve(Desc.iNumPositions);

	for (_uint i = 0; i < Desc.iNumPositions; ++i)
	{
		const _float3& vPosition = Desc.pPositions[i];
		const _float fYaw = Resolve_GrassYaw(i, vPosition);

		_matrix World =
			XMMatrixScaling(FIELD_GRASS_SCALE, FIELD_GRASS_SCALE, FIELD_GRASS_SCALE) *
			XMMatrixRotationY(fYaw) *
			XMMatrixTranslation(vPosition.x, vPosition.y, vPosition.z);

		VTXFIELDGRASS_INSTANCE Instance{};
		XMStoreFloat4(&Instance.vRight, World.r[0]);
		XMStoreFloat4(&Instance.vUp, World.r[1]);
		XMStoreFloat4(&Instance.vLook, World.r[2]);
		XMStoreFloat4(&Instance.vTranslation, World.r[3]);

		m_Instances.emplace_back(Instance);
	}

	return S_OK;
}

CFieldGrassBatch* CFieldGrassBatch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFieldGrassBatch* pInstance = new CFieldGrassBatch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFieldGrassBatch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFieldGrassBatch::Clone(void* pArg)
{
	CFieldGrassBatch* pInstance = new CFieldGrassBatch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFieldGrassBatch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFieldGrassBatch::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	
	__super::Free();
}