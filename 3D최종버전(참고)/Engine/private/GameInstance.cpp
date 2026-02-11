#include "GameInstance.h"

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Prototype_Manager.h"
#include "Renderer.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Target_Manager.h"
#include "Shadow.h"
#include "Picking.h"
#include "Frustum.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	
	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;


	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;


	m_pShadow = CShadow::Create();
	if (nullptr == m_pShadow)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pPicking->Update();

	m_pInput_Device->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);

	/* 저장되어있는 뷰, 투영행렬의 역행렬을 다 구해둔다. */
	/* 카메라 위치도 구해둔다 .*/
	m_pPipeLine->Update();

	m_pFrustum->Update();
	
	m_pObject_Manager->Update(fTimeDelta);
	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);


}

HRESULT CGameInstance::Begin_Draw(const _float4* pColor)
{
	m_pGraphic_Device->Clear_BackBuffer_View(pColor);
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	
	m_pRenderer->Draw();

	m_pLevel_Manager->Render();

	return S_OK;
}

HRESULT CGameInstance::End_Draw()
{
	m_pGraphic_Device->Present();

	return S_OK;
}

void CGameInstance::Clear_Resources(_uint iLevelID)
{
	m_pPrototype_Manager->Clear(iLevelID);
	m_pObject_Manager->Clear(iLevelID);
}

_float CGameInstance::Random(_float fMin, _float fMax)
{
	_float		fRandomNormal = static_cast<_float>(rand()) / RAND_MAX;
	
	return fMin + fRandomNormal * (fMax - fMin);
}

#pragma endregion


#pragma region INPUT_DEVICE

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

#pragma endregion

#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const _wstring& pTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& pTimerTag)
{
	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

void CGameInstance::Update_TimeDelta(const _wstring& pTimerTag)
{
	m_pTimer_Manager->Update_TimeDelta(pTimerTag);
}
#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(_uint iLevelID, CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelID, pNewLevel);
}
#pragma endregion

#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::Add_Prototype(_uint iLevelID, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iLevelID, strPrototypeTag, pPrototype);;
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE eType, _uint iLevelID, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(eType, iLevelID, strPrototypeTag, pArg);
}
#pragma endregion

#pragma region OBJECT_MANAGER
CComponent* CGameInstance::Get_Component(_uint iLayerLevelID, const _wstring& strLayerTag, _uint iIndex, const _wstring& strComponentTag)
{
	return m_pObject_Manager->Get_Component(iLayerLevelID, strLayerTag, iIndex, strComponentTag);
}
HRESULT CGameInstance::Add_GameObject(_uint iPrototypeLevelID, const _wstring& strPrototypeTag, _uint iLayerLevelID, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject(iPrototypeLevelID, strPrototypeTag, iLayerLevelID, strLayerTag, pArg);
}
#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderGroup(RENDERGROUP eGroupID, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderGroup(eGroupID, pRenderObject);
}

#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugComponent(CComponent* pDebugComponent)
{
	return m_pRenderer->Add_DebugComponent(pDebugComponent);
}
#endif

#pragma endregion

#pragma region PIPELINE
const _float4x4* CGameInstance::Get_Transform_Float4x4_Ptr(D3DTS eState)
{

	return m_pPipeLine->Get_Transform_Float4x4_Ptr(eState);
}
_matrix CGameInstance::Get_Transform_Matrix(D3DTS eState)
{
	return m_pPipeLine->Get_Transform_Matrix(eState);	
}

const _float4x4* CGameInstance::Get_Transform_Float4x4_Inverse_Ptr(D3DTS eState)
{
	return m_pPipeLine->Get_Transform_Float4x4_Inverse_Ptr(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(D3DTS eState)
{
	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

const _float4* CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_Transform(D3DTS eState, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

#pragma endregion

#pragma region LIGHT_MANAGER

const LIGHT_DESC* CGameInstance::Get_Light(_uint iIndex)
{
	return m_pLight_Manager->Get_Light(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

#pragma endregion





#pragma region FONT_MANAGER
HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}


HRESULT CGameInstance::Draw_Font(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor)
{
	return m_pFont_Manager->Draw(strFontTag, strText, vPosition, vColor);
}

#pragma endregion
#pragma region TARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{

	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, _bool isClearTarget, _bool isClearDepth, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, isClearTarget, isClearDepth, pDSV);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(strTargetTag, pShader, pConstantName);
}

HRESULT CGameInstance::Copy_RT_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pOut)
{
	return m_pTarget_Manager->Copy_RT_Resource(strTargetTag, pOut);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Render_MRT(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_MRT(strMRTTag, pShader, pVIBuffer);
}

#pragma endregion

#pragma region SHADOW

HRESULT CGameInstance::Add_Shadow_Light(const SHADOW_DESC& ShadowDesc)
{
	return m_pShadow->Add_Shadow_Light(ShadowDesc);
}
const _float4x4* CGameInstance::Get_Shadow_Transform_Float4x4_Ptr(D3DTS eState)
{
	return m_pShadow->Get_Shadow_Transform_Float4x4_Ptr(eState);
}
#pragma endregion

#pragma region PICKING

_bool CGameInstance::Picking(_float3* pOut)
{
	return m_pPicking->Picking(pOut);
}
#pragma endregion

#pragma region FRUSTUM
void CGameInstance::Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse)
{
	m_pFrustum->Transform_ToLocalSpace(WorldMatrixInverse);
}
_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRadius)
{
	return m_pFrustum->isIn_WorldSpace(vWorldPos, fRadius);
}
_bool CGameInstance::isIn_Frustum_LocalSpace(_fvector vLocalPos, _float fRadius)
{
	return m_pFrustum->isIn_LocalSpace(vLocalPos, fRadius);
}
#endif

#pragma endregion

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pFrustum);
	Safe_Release(m_pPicking);
	Safe_Release(m_pShadow);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);	
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pTimer_Manager);

	DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();


}
