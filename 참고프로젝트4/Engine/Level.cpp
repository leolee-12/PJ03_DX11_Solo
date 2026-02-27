#include "..\Public\Level.h"
#include "GameInstance.h"

CLevel::CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	 
	 
}

HRESULT CLevel::Initialize()
{
	//m_pGameInstance->Clear_TickEvents();
	//m_pGameInstance->Reset_CollisionManager();

	if (FAILED(Ready_Light()))
		RETURN_EFAIL;

	if (FAILED(Ready_Layer_Camera(L_CAMERA)))
		RETURN_EFAIL;

	if (FAILED(Ready_Layer_Environment(L_ENVIRONMENT)))
		RETURN_EFAIL;

	if (FAILED(Ready_Layer_Collider(L_COLLIDER)))
		RETURN_EFAIL;

	if (FAILED(Ready_Layer_Object(L_OBJECT)))
		RETURN_EFAIL;

	if (FAILED(Ready_Layer_Player(L_PLAYER)))
		RETURN_EFAIL;
	if (FAILED(Ready_Layer_Monster(L_MONSTER)))
		RETURN_EFAIL;
	if (FAILED(Ready_Layer_NPC(L_NPC)))
		RETURN_EFAIL;

	if (FAILED(Ready_Layer_Effect(L_EFFECT)))
		RETURN_EFAIL;

	if (FAILED(Ready_Layer_Trigger(L_TRIGGER)))
		RETURN_EFAIL;

	if (FAILED(Ready_Layer_UI(L_UI)))
		RETURN_EFAIL;

	Ready_Event();



	//if (FAILED(Ready_Layer_UI(L_TRIGGER)))
	//	RETURN_EFAIL; 

	return S_OK;
}

void CLevel::Tick(_cref_time fTimeDelta)
{
}

void CLevel::Late_Tick(_cref_time fTimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

HRESULT CLevel::Ready_Light()
{

	return S_OK;
}

HRESULT CLevel::Ready_Layer_Environment(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel::Ready_Layer_Object(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel::Ready_Layer_Player(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel::Ready_Layer_Monster(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel::Ready_Layer_NPC(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel::Ready_Layer_Effect(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel::Ready_Layer_Camera(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel::Ready_Layer_UI(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel::Ready_Layer_Trigger(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel::Ready_Layer_Collider(const LAYERTYPE& strLayerTag)
{
	return S_OK;
}

void CLevel::Ready_Event()
{
}

void CLevel::Free()
{ 
 	Safe_Release(m_pGameInstance);
}
