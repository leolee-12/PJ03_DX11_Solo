#include "pch.h"
#include "CGrid.h"
#include "CProtoMgr.h"
#include "CManagement.h"

CGrid::CGrid(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CGrid::CGrid(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CGrid::~CGrid()
{
}

HRESULT CGrid::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CGrid::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

void CGrid::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGrid::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	//m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

HRESULT CGrid::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// TerrainTex
	//pComponent = m_pBufferCom = dynamic_cast<Engine::CGridCol*>
	//	(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GridCol"));
	//
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//
	//m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}

CGrid* CGrid::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGrid* pTerrain = new CGrid(pGraphicDev);

	if (FAILED(pTerrain->Ready_GameObject()))
	{
		Safe_Release(pTerrain);
		MSG_BOX("pTerrain Create Failed");
		return nullptr;
	}

	return pTerrain;
}

void CGrid::Free()
{
	//Safe_Release(m_pBufferCom);

	CGameObject::Free();
}
