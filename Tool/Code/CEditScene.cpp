#include "pch.h"
#include "CEditScene.h"
#include "CProtoMgr.h"
#include "CLightMgr.h"

CEditScene::CEditScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CEditScene::~CEditScene()
{
}

HRESULT CEditScene::Ready_Scene()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Camera_Layer(L"Camera_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	// 임시 코드

	_matrix matView, matProj;

	_vec3   vEye{ 60.f, 50.f, 0.f };
	_vec3   vAt{ 60.f, 0.f, 60.f };
	_vec3   vUp{ 0.f, 1.f, 0.f };

	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.f), (_float)WINCX / WINCY, 0.1f, 1000.f);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	return S_OK;
}

_int CEditScene::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	return iExit;
}

void CEditScene::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CEditScene::Render_Scene()
{
}

HRESULT CEditScene::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	//CGameObject* pGameObject = nullptr;
	//
	// SkyBox
	//pGameObject = CSkyBox::Create(m_pGraphicDev);
	//
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//
	//if (FAILED(pLayer->Add_GameObject(L"SkyBox", pGameObject)))
	//	return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CEditScene::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	//CGameObject* pGameObject = nullptr;
	//
	// Terrain
	//pGameObject = CTerrain::Create(m_pGraphicDev);
	//
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//
	//if (FAILED(pLayer->Add_GameObject(L"Terrain", pGameObject)))
	//	return E_FAIL;
	//
	//// Player
	//pGameObject = CPlayer::Create(m_pGraphicDev);
	//
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//
	//if (FAILED(pLayer->Add_GameObject(L"Player", pGameObject)))
	//	return E_FAIL;
	//
	// Monster
	//pGameObject = CMonster::Create(m_pGraphicDev);
	//
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//
	//if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
	//	return E_FAIL;
	//
	//for (_int i = 0; i < 500; ++i)
	//{
	//	pGameObject = CEffect::Create(m_pGraphicDev);
	//
	//	if (nullptr == pGameObject)
	//		return E_FAIL;
	//
	//	if (FAILED(pLayer->Add_GameObject(L"Effect", pGameObject)))
	//		return E_FAIL;
	//}

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CEditScene::Ready_Camera_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	//CGameObject* pGameObject = nullptr;
	//
	//_vec3   vEye{ 0.f, 10.f, -10.f };
	//_vec3   vAt{ 0.f, 0.f, 1.f };
	//_vec3   vUp{ 0.f, 1.f, 0.f };
	//
	// DynamicCamera
	//pGameObject = CDynamicCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);
	//
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//
	//if (FAILED(pLayer->Add_GameObject(L"DynamicCamera", pGameObject)))
	//	return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CEditScene::Ready_UI_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;


	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CEditScene::Ready_Light()
{
	D3DLIGHT9	tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tLightInfo.Direction = { 1.f, -1.f, 1.f };

	if (FAILED(CLightMgr::GetInstance()->Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;

	return S_OK;
}

CEditScene* CEditScene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEditScene* pLogo = new CEditScene(pGraphicDev);

	if (FAILED(pLogo->Ready_Scene()))
	{
		Safe_Release(pLogo);
		MSG_BOX("pLogo Create Failed");
		return nullptr;
	}

	return pLogo;
}

void CEditScene::Free()
{
	CScene::Free();
}
