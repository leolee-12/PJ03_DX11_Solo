#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Renderer.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Target_Manager.h"

#include "Camera.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

#pragma region ENGINE

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_vViewportDesc = _float2(	static_cast<_float>(EngineDesc.iViewportWidth),
								static_cast<_float>(EngineDesc.iViewportHeight));

	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd,
												EngineDesc.eWinMode,
												EngineDesc.iViewportWidth,
												EngineDesc.iViewportHeight,
												ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pInput_Device->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);
	m_pObject_Manager->Update(fTimeDelta);
	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Begin_Draw()
{
	_float4 vColor = _float4(0.f, 0.f, 1.f, 1.f);

	if (FAILED(m_pGraphic_Device->Clear_BackBuffer_View(&vColor)))
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	if (FAILED(m_pRenderer->Draw()))
		return E_FAIL;

	if (FAILED(m_pLevel_Manager->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::End_Draw()
{
	return m_pGraphic_Device->Present();
}

void CGameInstance::Clear_Resources(_int iLevelIndex)
{
	if (-1 == iLevelIndex)
		return;

	/* iLevelIndex용 자원을 정리 */
	m_pObject_Manager->Clear(iLevelIndex);
	m_pPrototype_Manager->Clear(iLevelIndex);
}

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pMainCamera);

	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pGraphic_Device);

	DestroyInstance();
}

const HWND CGameInstance::Get_HWND() const
{
	return m_pGraphic_Device->Get_HWND();
}

_float CGameInstance::Random(_float fMin, _float fMax)
{
	return fMin + static_cast<_float>(rand()) / RAND_MAX * (fMax - fMin);
}
#pragma endregion

#pragma region TIMER_MANAGER

HRESULT CGameInstance::Add_Timer(WNameID strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

float CGameInstance::Compute_Timer(WNameID strTimerTag)
{
	return m_pTimer_Manager->Compute_Timer(strTimerTag);
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(_int iNewLevelIndex, CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(iNewLevelIndex, pNewLevel);
}

_int CGameInstance::Get_CurrentLevel() const
{
	return m_pLevel_Manager->Get_CurrentLevel();
}
#pragma endregion

#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, WNameID strProtoTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iLevelIndex, strProtoTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE eType, _uint iLevelIndex, WNameID strProtoTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(eType, iLevelIndex, strProtoTag, pArg);
}
#pragma endregion

#pragma region OBJECT_MANAGER
CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const WNameID strLayerTag, const WNameID strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}
HRESULT CGameInstance::Add_GameObject(_uint iPrototypeLevelIndex, WNameID strProtoTag, _uint iLayerLevelIndex, WNameID strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, strProtoTag, iLayerLevelIndex, strLayerTag, pArg);
}

HRESULT CGameInstance::Add_GameObject_Ex(_uint iLayerLevel, WNameID strLayerTag, CGameObject* pObj)
{
	return m_pObject_Manager->Add_GameObject_Ex(iLayerLevel, strLayerTag, pObj);
}

const list<class CGameObject*>* CGameInstance::Get_ObjectList(_uint iLevel, WNameID strLayerTag)
{
	return m_pObject_Manager->Get_ObjectList(iLevel, strLayerTag);
}

vector<CGameObject*> CGameInstance::Get_LevelObjects(_uint iLevel) const
{
	return m_pObject_Manager->Get_LevelObjects(iLevel);
}
#pragma endregion

#pragma region RENDERER
void CGameInstance::Add_RenderGroup(RENDERID eGroupID, CGameObject* pGameObject)
{
	m_pRenderer->Add_RenderGroup(eGroupID, pGameObject);
}

#ifdef _DEBUG
void CGameInstance::Add_DebugComponent(CComponent* pComponent)
{
	m_pRenderer->Add_DebugComponent(pComponent);
}
#endif
#pragma endregion

#pragma region PIPELINE
const _float4x4* CGameInstance::Get_Transform(D3DTS eState) const
{
	return m_pPipeLine->Get_Transform(eState);
}

const _float4x4* CGameInstance::Get_Transform_Inverse(D3DTS eState) const
{
	return m_pPipeLine->Get_Transform_Inverse(eState);
}

const _float4* CGameInstance::Get_CamPosition() const
{
	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_CameraWorld(_fmatrix StateMatrix)
{
	m_pPipeLine->Set_CameraWorld(StateMatrix);
}

void CGameInstance::Set_Projection(_fmatrix StateMatrix)
{
	m_pPipeLine->Set_Projection(StateMatrix);
}
void CGameInstance::Set_MainCamera(CCamera* pCamera)
{
	if (nullptr != m_pMainCamera)
		Safe_Release(m_pMainCamera);

	m_pMainCamera = pCamera;
	Safe_AddRef(m_pMainCamera);
}
CCamera* CGameInstance::Get_MainCamera() const
{
	return m_pMainCamera;
}
void CGameInstance::Toggle_CameraFollow()
{
	m_pMainCamera->Toggle_Following();
}
#pragma endregion

#pragma region INPUT_DEVICE
void CGameInstance::Set_InputState(INPUT_STATE eState)
{
	m_pInput_Device->Set_InputState(eState);
}

INPUT_STATE CGameInstance::Get_InputState() const
{
	return m_pInput_Device->Get_InputState();
}

_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Down(byKeyID);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Up(byKeyID);
}

_bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	return m_pInput_Device->Key_Pressing(byKeyID);
}

_bool CGameInstance::Mouse_Down(DIMB eMouse)
{
	return m_pInput_Device->Mouse_Down(eMouse);
}

_bool CGameInstance::Mouse_Up(DIMB eMouse)
{
	return m_pInput_Device->Mouse_Up(eMouse);
}

_bool CGameInstance::Mouse_Pressing(DIMB eMouse)
{
	return m_pInput_Device->Mouse_Pressing(eMouse);
}

_long CGameInstance::Mouse_Move(DIMM eMouseState)
{
	return m_pInput_Device->Mouse_Move(eMouseState);
}
#pragma endregion

#pragma region LIGHT_MANAGER

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

HRESULT CGameInstance::Render_Light(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}
#pragma endregion

#pragma region FONT_MANAGER
HRESULT CGameInstance::Add_Font(const WNameID strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

_float2 CGameInstance::Measure_Text(const WNameID strFontTag, const _tchar* pText)
{
	return m_pFont_Manager->Measure_Text(strFontTag, pText);
}

HRESULT XM_CALLCONV CGameInstance::Draw_Text(const WNameID strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, const _float2& vScale)
{
	return m_pFont_Manager->Draw(strFontTag, pText, vPosition, vColor, fRotation, vOrigin, vScale);
}
#pragma endregion

#pragma region TARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(WNameID strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(WNameID strMRTTag, WNameID strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(WNameID strMRTTag)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(WNameID strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(strTargetTag, pShader, pConstantName);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(WNameID strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_RT_Debug(WNameID strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif
#pragma endregion

void CGameInstance::Free()
{
	__super::Free();
}
