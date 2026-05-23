#include "MapObject.h"

#include "GameInstance.h"
#include "Mesh.h"

#ifdef _DEBUG
_bool CMapObject::s_bDebugCullingEnabled = true;
_bool CMapObject::s_bDebugCullLogEnabled = false;
#endif

namespace
{
	_bool XM_CALLCONV Is_MeshAABBVisible(CGameInstance* pGI, const BoundingBox& LocalAABB, _fmatrix WorldMatrix)
	{
		if (nullptr == pGI)
			return true;

		XMFLOAT3 LocalCorners[BoundingBox::CORNER_COUNT] = {};
		XMFLOAT3 WorldCorners[BoundingBox::CORNER_COUNT] = {};

		LocalAABB.GetCorners(LocalCorners);

		for (_uint i = 0; i < BoundingBox::CORNER_COUNT; ++i)
		{
			XMStoreFloat3(
				&WorldCorners[i],
				XMVector3TransformCoord(XMLoadFloat3(&LocalCorners[i]), WorldMatrix));
		}

		return pGI->isIn_Frustum_WorldSpace_AABB(WorldCorners, BoundingBox::CORNER_COUNT);
	}

#ifdef _DEBUG
	void Debug_LogMapCull(WNameID strModelTag, _uint iTotal, _uint iVisible, _uint iDrawn)
	{
		if (false == CMapObject::Debug_IsCullLogEnabled())
			return;

		static unordered_map<WNameID, _uint> s_LogFrameByModel;

		_uint& iFrame = s_LogFrameByModel[strModelTag];
		if ((iFrame++ % 120) != 0)
			return;

		wchar_t szLog[256] = {};
		swprintf_s(
			szLog,
			L"[MapCull] model=%ls(%u) visible=%u/%u drawn=%u skipped=%u\n",
			Engine::WNameRegistry::Lookup(strModelTag),
			strModelTag,
			iVisible,
			iTotal,
			iDrawn,
			iTotal - iDrawn);

		OutputDebugStringW(szLog);
	}
#endif
}

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MAPOBJECT_DESC& tDesc)
	: CGameObject{ pDevice, pContext }
	, m_tDesc{ tDesc }
{
	m_strName = L"Map_" + to_wstring(tDesc.strModelTag);
}

HRESULT CMapObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(m_RenderProfile.Build(m_pModelCom, m_tDesc.pRenderRule)))
		return E_FAIL;

	if (PROTO_COM_MODEL_MAP_TOWN02 == m_tDesc.strModelTag)
	{
		_vector vPos = { -0.95f, 2.f, 80.75f, 1.f };
		m_pTransformCom->Set_State(STATE::POSITION, vPos);
	}

	return S_OK;
}

void CMapObject::Priority_Update(_float fTimeDelta)
{

}

void CMapObject::Update(_float fTimeDelta)
{
}

void CMapObject::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDERID::SHADOW, this);
}

HRESULT CMapObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	const size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	_uint iVisibleMeshes = 0;

	vector<_bool> VisibleMask;
	VisibleMask.assign(iNumMeshes, true);

	_matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		CMesh* pMesh = m_pModelCom->Get_Mesh(i);
		if (nullptr == pMesh)
			return E_FAIL;

		VisibleMask[i] = Is_MeshAABBVisible(
			m_pGameInstance,
			pMesh->Get_LocalAABB(),
			WorldMatrix);

		if (true == VisibleMask[i])
			++iVisibleMeshes;
	}

	vector<CRenderProfile::MATERIAL_SLOT> Slots =
	{
		{ MATERIAL_TYPE::DIFFUSE, "g_TexDiff", 0 },
		{ MATERIAL_TYPE::DIFFUSE, "g_TexDiff2", 1 },
		{ MATERIAL_TYPE::DIFFUSE, "g_TexDiff3", 2 },
		{ MATERIAL_TYPE::DIFFUSE, "g_TexDiff4", 3 },
		{ MATERIAL_TYPE::OPACITY, "g_TexOpct" },
		{ MATERIAL_TYPE::UNKNOWN, "g_TexData" },
		{ MATERIAL_TYPE::UNKNOWN, "g_TexMask" },
	};

	_uint iDrawnMeshes = 0;

#ifdef _DEBUG
	const vector<_bool>* pVisibleMask =
		(true == s_bDebugCullingEnabled ? &VisibleMask : nullptr);
#else
	const vector<_bool>* pVisibleMask = &VisibleMask;
#endif

	if (FAILED(m_RenderProfile.Bind_AndDraw(
		m_pShaderCom,
		Slots,
		nullptr,
		pVisibleMask,
		&iDrawnMeshes)))
		return E_FAIL;

#ifdef _DEBUG
	Debug_LogMapCull(
		m_tDesc.strModelTag,
		static_cast<_uint>(iNumMeshes),
		iVisibleMeshes,
		iDrawnMeshes);
#endif

	return S_OK;
}

HRESULT CMapObject::Render_Shadow()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResourceWIT(m_pShaderCom, "g_WITMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::PROJ))))
		return E_FAIL;

	size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapObject::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), PROTO_COM_SHADER_MAP,
		COM_SHADER, reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_tDesc.iModelLevelIndex, m_tDesc.strModelTag,
		COM_MODEL, reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::Bind_ShaderResources()
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


CMapObject* CMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MAPOBJECT_DESC& tDesc)
{
	CMapObject* pInstance = new CMapObject(pDevice, pContext, tDesc);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject::Clone(void* pArg)
{
	CMapObject* pInstance = new CMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
