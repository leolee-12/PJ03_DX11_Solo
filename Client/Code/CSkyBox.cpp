#include "pch.h"	//	(P)
#include "CSkyBox.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CSkyBox::CSkyBox(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CSkyBox::~CSkyBox()
{
}

HRESULT CSkyBox::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(40.f, 40.f, 40.f); // SkyBox 크기 조정

	return S_OK;
}

_int CSkyBox::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

void CSkyBox::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	_matrix		matCamWorld;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
	D3DXMatrixInverse(&matCamWorld, 0, &matCamWorld);	// 카메라의 월드행렬 = 뷰스페이스변환행렬의 역행렬

	m_pTransformCom->Set_Pos(matCamWorld._41, matCamWorld._42 + 3.f, matCamWorld._43);
	// SkyBox를 벗어나지 않도록 SkyBox의 위치를 매 프레임 카메라의 위치로 갱신해준다
	// y + 3.f? : 배경 느낌을 위해 살짝 위로 올린 것
}

void CSkyBox::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// CULLMODE : NONE vs CW
	// - 매우 미미한 차이이지만, 컬링 여부를 판단하는 비용보다 더 많은 삼각형을 레스터라이즈하는 비용이 좀 더 큼
	// - 불필요한 부분(외부에서 바라보는 면)을 아예 그리지 않는 CW가 미세하게 더 유리하지만, 무시할 수 있는 차이
	// - 그러나, 확실히 winding order를 파악하고 있다면 괜찮지만, 간편하게 NONE으로 사용하기도 함(큰 차이 없어서)
	
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	// Z버퍼 정렬
	// - 지형이 스카이박스 경계에 걸쳐 있을 때, 카메라 무빙 시 지형이 생겨나거나, 사라지는 것처럼 보임
	// - Z버퍼 정렬하여 자동으로 뒤쪽의 물체는 그리지 않음
	// - 그러나, 스카이박스는 제일 먼저 그리므로 Z버퍼 정렬이 필요하지 않음

	m_pTextureCom->Set_Texture(3);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	// 스카이박스를 그린 뒤 Culling과 z버퍼 정렬을 다시 켜준다.


}

HRESULT CSkyBox::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// CubeTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CCubeTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SkyTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CSkyBox* CSkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkyBox* pSkyBox = new CSkyBox(pGraphicDev);

	if (FAILED(pSkyBox->Ready_GameObject()))
	{
		Safe_Release(pSkyBox);
		MSG_BOX("pSkyBox Create Failed");
		return nullptr;
	}

	return pSkyBox;
}

void CSkyBox::Free()
{
	Safe_Release(m_pBufferCom);

	CGameObject::Free();
}
