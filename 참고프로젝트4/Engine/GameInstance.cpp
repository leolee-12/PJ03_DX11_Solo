#include "GameInstance.h"

#include "Graphic_Device.h"
#include "TImer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Event_Manager.h"
#include "Collision_Manager.h"
#include "Light_Manager.h"
#include "Target_Manager.h"
#include "Font_Manager.h"
#include "Renderer.h"
#include "Input_Device.h"
#include "Texture_Manager.h"
#include "Model_Manager.h"
#include "Shader_Manager.h"
#include "Texture_Data.h"
#include "Sound_Manager.h"
#include "Utility_File.h"
#include "PhysX_Manager.h"
#include "Key_Manager.h"
#include "ThreadPool.h"
#include "Frustum.h"
#include "SoundPoint_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

_float CGameInstance::m_fAdjustTimeDelta = 1.f;
_bool CGameInstance::m_bPauseGame = false;
_bool CGameInstance::m_bIsFocusedWindow = true;

CGameInstance::CGameInstance()
{
}

/* For.Engine */

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, _uint iNumColLayers, HINSTANCE hInstance, const GRAPHIC_DESC& GraphicDesc, _Inout_ ComPtr<ID3D11Device>* ppDevice, _Inout_ ComPtr<ID3D11DeviceContext>* ppContext)
{
	m_strMainBinPath = TEXT("../Bin/Resources/");
	m_strMainResourcePath = TEXT("../Bin/");
	m_strMainDataPath = TEXT("../Bin/Data/");
	m_strMainJSONScriptPath = TEXT("../Bin/Data/JSON_Script/");

	/* 그래픽 디바이스를 초기화 하자.*/
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		RETURN_EFAIL;

	m_pInput_Device = CInput_Device::Create(hInstance, GraphicDesc.hWnd, GraphicDesc.iBackBufferSizeX, GraphicDesc.iBackBufferSizeY);
	if (nullptr == m_pInput_Device)
		RETURN_EFAIL;

	/* 타이머를 사용할 준비를 하자. */
	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		RETURN_EFAIL;

	m_pTexture_Manager = CTexture_Manager::Create(ppDevice->Get(), ppContext->Get(), TEXT("../Bin/Resources/Textures/"));
	if (nullptr == m_pTexture_Manager)
		RETURN_EFAIL;

	m_pModel_Manager = CModel_Manager::Create(ppDevice->Get(), ppContext->Get(), TEXT("../Bin/Resources/Model/"));
	if (nullptr == m_pModel_Manager)
		RETURN_EFAIL;
	
	m_pShader_Manager = CShader_Manager::Create(ppDevice->Get(), ppContext->Get(), TEXT("../Bin/ShaderFiles/"));
	if (nullptr == m_pShader_Manager)
		RETURN_EFAIL;

	m_pSound_Manager = CSound_Manager::Create("../Bin/Resources/Sounds/");
	if (nullptr == m_pShader_Manager)
		RETURN_EFAIL;
	
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		RETURN_EFAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		RETURN_EFAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		RETURN_EFAIL;

	m_pEvent_Manager = CEvent_Manager::Create(iNumLevels);
	if (nullptr == m_pEvent_Manager)
		RETURN_EFAIL;

	m_pCollision_Manager = CCollision_Manager::Create(iNumColLayers);
	if (nullptr == m_pCollision_Manager)
		RETURN_EFAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		RETURN_EFAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		RETURN_EFAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		RETURN_EFAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		RETURN_EFAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		RETURN_EFAIL;

	m_pKey_Manager = CKey_Manager::Create(m_pInput_Device);
	if (nullptr == m_pKey_Manager)
		RETURN_EFAIL;
	
	m_pThreadPool = CThreadPool::Create(0, true);
	if (nullptr == m_pThreadPool)
		RETURN_EFAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		RETURN_EFAIL;

	m_pSoundPoint_Manager = CSoundPoint_Manager::Create();
	if (nullptr == m_pSoundPoint_Manager)
		RETURN_EFAIL;

	/*RandomDevice*/
	
	m_RandomNumber = mt19937_64(m_RandomDevice());

	//PostQuitMessage(0);
	
	return S_OK;
}

void CGameInstance::Tick_Engine(_cref_time fTimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager ||
		nullptr == m_pPipeLine ||
		nullptr == m_pEvent_Manager ||
		nullptr == m_pInput_Device)
		return;


	m_pInput_Device->Update_InputDev();

	m_pKey_Manager->Update_KeyState();

	m_pLevel_Manager->Tick(fTimeDelta);

	m_pObject_Manager->Priority_Tick(fTimeDelta);

	m_pObject_Manager->Tick(fTimeDelta);

	m_pLight_Manager->Tick(fTimeDelta);

	m_pSoundPoint_Manager->Tick(fTimeDelta);

	m_pLevel_Manager->Late_Tick(fTimeDelta);

	m_pObject_Manager->Late_Tick(fTimeDelta);

	m_pSound_Manager->Tick(fTimeDelta);

	m_pPipeLine->Tick();

	m_pFrustum->Tick();



	if (!CGameInstance::m_bPauseGame)
		m_pEvent_Manager->Tick(fTimeDelta);

	//m_pCollision_Manager->Tick();

	//Transform 결과값 PhysX에 저장 + Renderer에 오브젝트 추가
	m_pObject_Manager->Before_Render(fTimeDelta);

	GET_SINGLE(CPhysX_Manager)->Tick(fTimeDelta);
	//m_pInput_Device->LateUpdate_InputDev();
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager||
		nullptr == m_pEvent_Manager ||
		nullptr == m_pCollision_Manager || 
		nullptr == m_pSoundPoint_Manager)
		return;

	/* 오브젝트 매니져에 레벨별로 구분해 놓은 객체들 중 특정된 사본 객체들을 지운다.  */
	m_pObject_Manager->Clear(iLevelIndex);

	/* 컴포넌트 매니져에 레벨별로 구분해 놓은 컴포넌트들 중 특정된 원형 객체들을 지운다.  */
	m_pComponent_Manager->Clear(iLevelIndex);

	m_pEvent_Manager->Clear(iLevelIndex);

	//m_pCollision_Manager->Clear();

	//m_pSoundPoint_Manager->Reset_SoundPoint();
}

HRESULT CGameInstance::Render_Engine()
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pRenderer)
		RETURN_EFAIL;
#ifdef _DEBUG
	if (nullptr != m_pSoundPoint_Manager)
		m_pSoundPoint_Manager->Render();
#endif
	m_pRenderer->Draw_RenderGroup();

#ifdef _DEBUG
	m_pLevel_Manager->Render();
#endif

	m_pInput_Device->LateUpdate_InputDev();

	return S_OK;
}


/* For.Graphic_Device */

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		RETURN_EFAIL;
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		RETURN_EFAIL;
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		RETURN_EFAIL;
	return m_pGraphic_Device->Present();
}

ComPtr<ID3D11RenderTargetView> CGameInstance::Get_BackBufferRTV() const
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_BackBufferRTV();
}

ComPtr<ID3D11DepthStencilView> CGameInstance::Get_DSV() const
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_DSV();
}

ComPtr<ID3D11ShaderResourceView> CGameInstance::Get_DS_SRV() const
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_DS_SRV();
}

ComPtr<ID3D11Device> CGameInstance::Get_D3D11Device() const
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_GraphicDev();
}

ComPtr<ID3D11DeviceContext> CGameInstance::Get_D3D11Context() const
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_GraphicContext();
}


/* For.Input_Device */

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{

	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}


bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device && nullptr ==  m_pKey_Manager)
		return false;

	if (m_pKey_Manager->Is_KeyRegistered(byKeyID))
		return m_pKey_Manager->Check_KeyState(byKeyID, KEYSTATE::KEY_PRESS);
	else
		return m_pInput_Device->Key_Pressing(byKeyID);
}

bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device && nullptr == m_pKey_Manager)
		return false;
	
	if (m_pKey_Manager->Is_KeyRegistered(byKeyID))
		return m_pKey_Manager->Check_KeyState(byKeyID, KEYSTATE::KEY_DOWN);
	else
		return m_pInput_Device->Key_Down(byKeyID);
}

bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device && nullptr == m_pKey_Manager)
		return false;

	if (m_pKey_Manager->Is_KeyRegistered(byKeyID))
		return m_pKey_Manager->Check_KeyState(byKeyID, KEYSTATE::KEY_UP);
	else
		return m_pInput_Device->Key_Up(byKeyID);
}

_bool CGameInstance::Key_PressingEx(_ubyte byKeyID, DIK_EX byModifierID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Key_PressingEx(byKeyID, byModifierID);
}

_bool CGameInstance::Key_DownEx(_ubyte byKeyID, DIK_EX byModifierID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Key_DownEx(byKeyID, byModifierID);
}

_bool CGameInstance::Key_UpEx(_ubyte byKeyID, DIK_EX byModifierID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Key_UpEx(byKeyID, byModifierID);
}

bool CGameInstance::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device && nullptr == m_pKey_Manager)
		return false;
	
	//return m_pInput_Device->Mouse_Pressing(eMouse);
	return m_pKey_Manager->Check_MouseState(eMouse, KEYSTATE::KEY_PRESS);
}

bool CGameInstance::Mouse_Down(MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device && nullptr == m_pKey_Manager)
		return false;
	
	//return m_pInput_Device->Mouse_Down(eMouse);
	return m_pKey_Manager->Check_MouseState(eMouse, KEYSTATE::KEY_DOWN);
}

bool CGameInstance::Mouse_Up(MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device && nullptr == m_pKey_Manager)
		return false;
	
	//return m_pInput_Device->Mouse_Up(eMouse);
	return m_pKey_Manager->Check_MouseState(eMouse, KEYSTATE::KEY_UP);
}

void CGameInstance::Mouse_Fix()
{
	if (nullptr == m_pInput_Device)
		return;
	m_pInput_Device->Mouse_Fix();
}

_bool CGameInstance::Mouse_RayIntersect(SPACETYPE eSpacetype, _float3* pOut, _fvector vV1, _fvector vV2, _fvector vV3, _matrix matWorld, _float* pLengthOut)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Mouse_RayIntersect(eSpacetype,pOut,vV1,vV2,vV3,matWorld,pLengthOut);
}

void CGameInstance::Mouse_ToWorld(_vector& vOrigin, _vector& vDir)
{
	if (nullptr == m_pInput_Device)
		return;
	return m_pInput_Device->Mouse_ToWorld(vOrigin,vDir);
}

_bool CGameInstance::Any_Input_Received()
{
	if (nullptr == m_pInput_Device)
		return false;

	return  m_pInput_Device->Any_Input_Received();
}

/* For.Key_Manager*/

void CGameInstance::Register_Key(_ubyte key)
{
	if (nullptr == m_pKey_Manager)
		return;
	
	return m_pKey_Manager->Register_Key(key);
}

_bool CGameInstance::Is_KeyRegistered(_ubyte key)
{
	if (nullptr == m_pKey_Manager)
		return false;

	return m_pKey_Manager->Is_KeyRegistered(key);
}

KEYSTATE CGameInstance::Get_KeyState(_ubyte key)
{
	if (nullptr == m_pKey_Manager)
		return KEYSTATE::KEY_NONE;

	return m_pKey_Manager->Get_KeyState(key);
}

_bool CGameInstance::Check_KeyState(_ubyte key, KEYSTATE keyState)
{
	if (nullptr == m_pKey_Manager)
		return false;

	return m_pKey_Manager->Check_KeyState(key,keyState);
}

KEYSTATE CGameInstance::Get_MouseState(MOUSEKEYSTATE mouse)
{
	if (nullptr == m_pKey_Manager)
		return KEYSTATE::KEY_NONE;

	return  m_pKey_Manager->Get_MouseState(mouse);
}

_bool CGameInstance::Check_MouseState(MOUSEKEYSTATE mouse, KEYSTATE keyState)
{
	if (nullptr == m_pKey_Manager)
		return false;

	return  m_pKey_Manager->Check_MouseState(mouse, keyState);
}


/* For.Timer_Manager */

HRESULT CGameInstance::Add_Timer(const wstring & strTimeTag)
{
	if (nullptr == m_pTimer_Manager)
		RETURN_EFAIL;

	return m_pTimer_Manager->Add_Timer(strTimeTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring & strTimeTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimeTag);
}

void CGameInstance::Save_TimeDelta(_float NowTime)
{
	m_fNowTimeDelta_60 = NowTime;
}

_float CGameInstance::Get_TimeDelta()
{
	return m_fNowTimeDelta_60;
}


/* For.Level_Manager */

HRESULT CGameInstance::Open_Level(_uint iCurrentLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		RETURN_EFAIL;

	return m_pLevel_Manager->Open_Level(iCurrentLevelIndex, pNewLevel);
}

_bool CGameInstance::IsVisitedLevel(_uint iCurrentLevelIndex)
{
	return m_pLevel_Manager->IsVisitedLevel(iCurrentLevelIndex);
}  

void CGameInstance::Set_VisitedLevel(_uint iCurrentLevelIndex)
{
	m_pLevel_Manager->Set_VisitedLevel(iCurrentLevelIndex);
}

_uint CGameInstance::Get_CreateLevelIndex()
{
	return m_pLevel_Manager->Get_CreateLevelIndex();
}

void CGameInstance::Set_CreateLevelIndex(_uint eLevelIndex)
{
	m_pLevel_Manager->Set_CreateLevelIndex(eLevelIndex);
}

CLevel* CGameInstance::Get_CurrentLevel()
{
	return m_pLevel_Manager->Get_CurrentLevel();
}

_uint CGameInstance::Get_CurrentLevelIndex()
{
	return m_pLevel_Manager->Get_CurrentLevelIndex();
}

/* For.Object_Manager */

HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, shared_ptr<CGameObject> pPrototype)
{
	if (nullptr == m_pObject_Manager)
		RETURN_EFAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}


HRESULT CGameInstance::Add_CloneObject(_uint iLevelIndex, const LAYERTYPE& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		RETURN_EFAIL;

	shared_ptr<CGameObject> ppOut;
	return m_pObject_Manager->Add_CloneObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg, &ppOut);
}

HRESULT CGameInstance::Add_Object(_uint iLevelIndex, const LAYERTYPE& strLayerTag, shared_ptr<CGameObject> pObject)
{
	if (nullptr == m_pObject_Manager)
		RETURN_EFAIL;

	return m_pObject_Manager->Add_Object(iLevelIndex, strLayerTag,pObject);
}

shared_ptr<CGameObject> CGameInstance::CloneObject(const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->CloneObject(strPrototypeTag,pArg);
}

map<const wstring,shared_ptr<CGameObject>>* CGameInstance::Get_ObjectPrototypeList()
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_PrototypeList();
}

list<shared_ptr<CGameObject>>* CGameInstance::Get_ObjectList(_uint iLevelIndex, const LAYERTYPE& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ObjectList(iLevelIndex,strLayerTag);
}

shared_ptr<CComponent> CGameInstance::Get_Component(_uint iLevelIndex, const LAYERTYPE& strLayerTag, const wstring& strComponentTag, _uint iIndex, const wstring& strPartTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag,iIndex,strPartTag);
}

shared_ptr<CGameObject> CGameInstance::Get_Object(_uint iLevelIndex, const LAYERTYPE& strLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Object(iLevelIndex, strLayerTag, iIndex);
}


/*For. Event_Manager*/

void CGameInstance::Add_LevelEvent(_uint iLevelIndex,string sEvent, function<_bool(weak_ptr<CBase>, shared_ptr<CGameObject>)> Func)
{
	if (nullptr == m_pEvent_Manager)
		return;
	return m_pEvent_Manager->Add_LevelEvent(iLevelIndex,sEvent, Func);
}

void CGameInstance::Erase_LevelEvent(_uint iLevelIndex, string sEvent)
{
	if (nullptr == m_pEvent_Manager)
		return;
	return m_pEvent_Manager->Erase_LevelEvent(iLevelIndex, sEvent);
}

HRESULT CGameInstance::Excute_LevelEvent(_uint iLevelIndex, string sEvent,weak_ptr<CBase> _pCaller, shared_ptr<CGameObject> pOther)
{
	if (nullptr == m_pEvent_Manager)
		RETURN_EFAIL;
	return m_pEvent_Manager->Excute_LevelEvent(iLevelIndex, sEvent, _pCaller, pOther);
}

void CGameInstance::Add_TickEvent(shared_ptr<CBase> _pSubscriber, function<_bool(_cref_time fTimeDelta)> Func)
{
	if (nullptr == m_pEvent_Manager)
		return;
	return m_pEvent_Manager->Add_TickEvent(_pSubscriber, Func);
}

void CGameInstance::Erase_TickEvent(shared_ptr<CBase> _pSubscriber)
{
	if (nullptr == m_pEvent_Manager)
		return;
	return m_pEvent_Manager->Erase_TickEvent(_pSubscriber);
}

void CGameInstance::Clear_TickEvents()
{
	if (nullptr == m_pEvent_Manager)
		return;
	return m_pEvent_Manager->Clear_TickEvents();
}


/* For.Component_Manager */

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<CComponent> pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		RETURN_EFAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

shared_ptr<CComponent> CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<CGameObject> pOwner,void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pOwner, pArg);
}

map<const wstring, shared_ptr<CComponent>>* CGameInstance::Get_ComponentPrototypeList(_uint iLevelIndex)
{
	if(nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Get_PrototypeList(iLevelIndex);
}


/* For.Collision_Manager */

void CGameInstance::Add_Collider(const _uint& iLayer, CCollider* pCollider)
{
	if (nullptr == m_pCollision_Manager)
		return;
 	return m_pCollision_Manager->Add_Collider(iLayer, pCollider);
}	

void CGameInstance::Add_LayerGroup(const _uint& iLeftLayer, const _uint& iRightLayer)
{
	if (nullptr == m_pCollision_Manager)
		return;
	return m_pCollision_Manager->Add_LayerGroup(iLeftLayer, iRightLayer);
}

void CGameInstance::Reset_CollisionManager()
{
	if (nullptr == m_pCollision_Manager)
		return;
	return m_pCollision_Manager->Clear();
}


/* For.Font_Manager */

HRESULT CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		RETURN_EFAIL;
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	if (nullptr == m_pFont_Manager)
		RETURN_EFAIL;
	return m_pFont_Manager->Render_Font(strFontTag, strText, vPosition, vColor, fScale, vOrigin, fRotation);
}


/* For.Renderer */

CRenderer* CGameInstance::Get_Renderer()
{
	return m_pRenderer;
}

HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eGroupID, shared_ptr<CGameObject> pGameObject)
{
	if (nullptr == m_pRenderer)
		RETURN_EFAIL;

	return m_pRenderer->Add_RenderGroup(eGroupID, pGameObject);
}

HRESULT CGameInstance::Add_DebugRender(shared_ptr<CComponent> pDebugCom)
{

	if (nullptr == m_pRenderer)
		RETURN_EFAIL;

	return m_pRenderer->Add_DebugRender(pDebugCom);
}

void CGameInstance::Set_RenderType(RENDERTYPE type)
{
	if (nullptr == m_pRenderer)
		return;

	return m_pRenderer->Set_RenderType(type);
}

#ifdef _DEBUG
void CGameInstance::Add_DebugEvent(FastDelegate0<HRESULT> Event)
{
	if (nullptr == m_pRenderer)
		return;

	return m_pRenderer->Add_DebugEvent(Event);
}

void CGameInstance::Toggle_DebugDraw()
{
	if (nullptr == m_pRenderer)
		return;

	return m_pRenderer->Toggle_DebugDraw();
}
#endif // _DEBUG



/* For.Target_Manager */


HRESULT CGameInstance::Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint iArraySize)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor,iArraySize);
}

HRESULT CGameInstance::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag, _bool bClear, ComPtr<ID3D11DepthStencilView> pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, bClear,pDSV);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RenderTarget_ShaderResource(const wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(strTargetTag, pShader, pConstantName);
}

CRenderTarget* CGameInstance::Find_RenderTarget(const wstring& strTargetTag)
{
	return m_pTarget_Manager->Find_RenderTarget(strTargetTag);
}

HRESULT CGameInstance::Bake_AntiAliasingDepthStencilView(_uint iWidth, _uint iHeight)
{
	return m_pTarget_Manager->Bake_AntiAliasingDepthStencilView(iWidth,iHeight);
}

ComPtr<ID3D11DepthStencilView> CGameInstance::Get_AntiAliasingDepthStencilView()
{
	return m_pTarget_Manager->Get_AntiAliasingDepthStencilView();
}

D3D11_VIEWPORT CGameInstance::Get_AntiAliasingViewPortDesc()
{
	return m_pTarget_Manager->Get_AntiAliasingViewPortDesc();
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RenderTarget_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Render_Debug_RTVs(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif


/* For.Light_Manager*/

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc, shared_ptr<CLight>* ppOut)
{
	return m_pLight_Manager->Add_Light(LightDesc, ppOut);
}

HRESULT CGameInstance::Add_Light(string FilePath, vector<shared_ptr<class CLight>>* LightVector)
{
	return m_pLight_Manager->Add_Light(FilePath, LightVector);

	return S_OK;
}

HRESULT CGameInstance::Render_Lights(shared_ptr<CShader> pShader, shared_ptr<CVIBuffer_Rect> pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}
void CGameInstance::Release_Light(shared_ptr<CLight> pLight)
{
	return m_pLight_Manager->Release_Light(pLight);
}
void CGameInstance::Reset_Light()
{
	return m_pLight_Manager->Reset_Light();
}
void CGameInstance::Reset_LightManager()
{
	return m_pLight_Manager->Clear();
}

const _bool CGameInstance::Get_Dead()
{
	return _bool();
}

shared_ptr<class CLight> CGameInstance::Make_Light_On_Owner(weak_ptr<CGameObject> _pOwner, _float4 vColor, _float fRange, _float3 vPosOffset, _bool bUseVolumetric, _float4 vAmbient)
{
	return m_pLight_Manager->Make_Light_On_Owner(_pOwner, vColor, fRange, vPosOffset, bUseVolumetric, vAmbient);
}

_float4x4 CGameInstance::Get_ShadowLight_ViewMatrix()
{
	return m_pLight_Manager->Get_ShadowLight_ViewMatrix();
}

_float4x4 CGameInstance::Get_ShadowLight_ProjMatrix()
{
	return m_pLight_Manager->Get_ShadowLight_ProjMatrix();
}

weak_ptr<class CLight> CGameInstance::Get_DirectionalLight()
{
	return m_pLight_Manager->Get_DirectionalLight();
}

shared_ptr<class CLight> CGameInstance::Find_Light(string LightName)
{
	return m_pLight_Manager->Find_Light(LightName);
}

void CGameInstance::Set_LightCulling(_bool bUseCull)
{
	if (nullptr == m_pLight_Manager)
		return;

	m_pLight_Manager->Set_Culling(bUseCull);
}


/* For.PipeLine */

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _float4x4 TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_matrix CGameInstance::Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrixInverse(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4Inverse(eState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_CamPosition(_float4 vCamPosition)
{
	if (nullptr == m_pPipeLine)
		return;

	return m_pPipeLine->Set_CamPosition(vCamPosition);
}

_float4 CGameInstance::Get_CamLook()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamLook();
}

void CGameInstance::Set_CamLook(_float4 vCamLook)
{
	if (nullptr == m_pPipeLine)
		return;

	return m_pPipeLine->Set_CamLook(vCamLook);
}

#pragma region Texture Manager
HRESULT CGameInstance::Ready_Texture(const wstring& strTexturePath, const _uint iNumTextures, const _bool bIsPermanent, _bool bUseMainPath)
{
	if (nullptr == m_pTexture_Manager)
		RETURN_EFAIL;

	return m_pTexture_Manager->Ready_Texture(strTexturePath, iNumTextures, bIsPermanent, bUseMainPath);
}

ID3D11ShaderResourceView* CGameInstance::Find_Front_SRV(const wstring& strTextureKey)
{
	if (nullptr == m_pTexture_Manager)
		return nullptr;

	return m_pTexture_Manager->Front_SRV(strTextureKey);
}

HRESULT CGameInstance::Include_All_Textures(const wstring& strMainPath, _bool bSavePath, const _bool bPermanent)
{
	if (nullptr == m_pTexture_Manager)
		RETURN_EFAIL;

	return m_pTexture_Manager->Include_All_Textures(strMainPath, bSavePath, bPermanent);
}

const map<wstring, shared_ptr<CTexture_Data>>* CGameInstance::Get_TexturesMap() const
{
	if (nullptr == m_pTexture_Manager)
		return nullptr;

	return m_pTexture_Manager->Get_TexturesMap();
}

CTexture_Manager* CGameInstance::Get_TextureManager()
{
	return m_pTexture_Manager;
}
#pragma endregion







#pragma region Model Manager
HRESULT CGameInstance::Load_Model(const wstring& strFileName)
{
	if (nullptr == m_pModel_Manager)
		RETURN_EFAIL;

	return m_pModel_Manager->Load_Model(strFileName);
}

HRESULT CGameInstance::Load_DirectorySub_Models(const wstring& strFolderPath,_bool bSavePath)
{
	if (nullptr == m_pModel_Manager)
		RETURN_EFAIL;

	return m_pModel_Manager->Load_DirectorySub_Models(strFolderPath, bSavePath);
}

HRESULT CGameInstance::Rebake_MaterialsAll(const wstring& strFolderPath)
{
	if (nullptr == m_pModel_Manager)
		RETURN_EFAIL;

	return m_pModel_Manager->Rebake_MaterialsAll(strFolderPath);
}

const FModelData* const CGameInstance::Find_ModelData(const wstring& strModelKey)
{
	if (nullptr == m_pModel_Manager)
		return nullptr;

	return m_pModel_Manager->Find_ModelData(strModelKey);
}

const FMeshData* const CGameInstance::Find_MeshData(const wstring& strModelKey, const wstring& strMeshKey, const _uint iRangeIndex)
{
	if (nullptr == m_pModel_Manager)
		return nullptr;

	return m_pModel_Manager->Find_MeshData(strModelKey, strMeshKey, iRangeIndex);
}

const FMeshData* const CGameInstance::Find_MeshData(const wstring& strModelKey, const _uint iIndex)
{
	if (nullptr == m_pModel_Manager)
		return nullptr;

	return m_pModel_Manager->Find_MeshData(strModelKey, iIndex);
}

const CMeshGroup* const CGameInstance::Find_MeshGroup(const wstring& strModelKey)
{
	if (nullptr == m_pModel_Manager)
		return nullptr;

	return m_pModel_Manager->Find_MeshGroup(strModelKey);
}

CBoneGroup* CGameInstance::Clone_BoneGroup(const wstring& strModelKey)
{
	if (nullptr == m_pModel_Manager)
		return nullptr;

	return m_pModel_Manager->Clone_BoneGroup(strModelKey);
}

CBoneGroup* CGameInstance::Find_BoneGroup(const wstring& strModelKey)
{
	if (nullptr == m_pModel_Manager)
		return nullptr;

	return m_pModel_Manager->Find_BoneGroup(strModelKey);
}

CBoneAnimGroup* CGameInstance::Find_AnimGroup(const wstring& strModelKey)
{
	if (nullptr == m_pModel_Manager)
		return nullptr;

	return m_pModel_Manager->Find_AnimGroup(strModelKey);
}

const FMaterialData* const CGameInstance::Find_MaterialData(const wstring& strModelKey, const _uint iIndex)
{
	if (nullptr == m_pModel_Manager)
		return nullptr;

	return m_pModel_Manager->Find_MaterialData(strModelKey, iIndex);
}
const unordered_map<wstring, FModelData*>* CGameInstance::Get_ModelDatas()
{
	if (nullptr == m_pModel_Manager)
		return nullptr;

	return &m_pModel_Manager->Get_ModelDatas();
}

#pragma endregion





#pragma region 셰이더 매니저
HRESULT CGameInstance::Load_Shader(const wstring& strFileName, const EShaderType eType, const _char* pEntryPoint)
{
	if (nullptr == m_pShader_Manager)
		return E_POINTER;

	return m_pShader_Manager->Load_Shader(strFileName, eType, pEntryPoint);
}


const ComPtr<ID3DBlob> CGameInstance::Get_ShaderByte(const EShaderType eType, const wstring& strKey)
{
	if (nullptr == m_pShader_Manager)
		return nullptr;

	return m_pShader_Manager->Find_ShaderByte(eType, strKey);
}



HRESULT CGameInstance::Load_Effect(const wstring& strFileName, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements
	, const D3D_SHADER_MACRO* pShaderMacro)
{
	if (nullptr == m_pShader_Manager)
		RETURN_EFAIL;

	return m_pShader_Manager->Load_Effect(strFileName, pElements, iNumElements, pShaderMacro);
}

HRESULT CGameInstance::Recompile_EffectsAll()
{
	if (nullptr == m_pShader_Manager)
		RETURN_EFAIL;

	return m_pShader_Manager->Recompile_EffectsAll();
}

ID3DX11Effect* CGameInstance::Find_Effect(const wstring& strEffectFileName) const
{
	if (nullptr == m_pShader_Manager)
		return nullptr;

	return m_pShader_Manager->Find_Effect(strEffectFileName);
}

shared_ptr<FEffectData> CGameInstance::Find_EffectData(const wstring& strEffectFileName, const D3D_SHADER_MACRO* pShaderMacro) const
{
	if (nullptr == m_pShader_Manager)
		return nullptr;

	return m_pShader_Manager->Find_EffectData(strEffectFileName, pShaderMacro);
}

CShader_Manager* CGameInstance::Get_ShaderManager()
{
	return m_pShader_Manager;
}
#pragma endregion



#pragma region 사운드 매니저

_int CGameInstance::Play_Sound(const wstring& strGroupKey, const wstring& strSoundKey, 
								ESoundGroup eSoundGroup, _float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return -1;

	return m_pSound_Manager->Play_Sound(strGroupKey, strSoundKey, eSoundGroup, fVolume);
}

_int CGameInstance::Play_3DSound(const wstring& strGroupKey, const wstring& strSoundKey,
									ESoundGroup eSoundGroup, _float fVolume, _float3 vPos)
{
	if (nullptr == m_pSound_Manager)
		return -1;

	return m_pSound_Manager->Play_3DSound(strGroupKey, strSoundKey, eSoundGroup, fVolume, vPos);
}

_int CGameInstance::Play_3DSound(const wstring& strGroupKey, vector<string> strSoundKeys, ESoundGroup eSoundGroup, shared_ptr<CGameObject> pOwner, _float3 vOffsetPos, _float fVolume, FMOD::Channel** ppOutChannel)
{
	if (nullptr == m_pSound_Manager)
		return -1;

	return m_pSound_Manager->Play_3DSound(strGroupKey, strSoundKeys, eSoundGroup, pOwner, vOffsetPos, fVolume, ppOutChannel);
}

_int CGameInstance::Play_3DSound(vector<string> strGroupKeys, vector<string> strSoundKeys, ESoundGroup eSoundGroup, shared_ptr<CGameObject> pOwner, _float3 vOffsetPos, _float fVolume, FMOD::Channel** ppOutChannel)
{
	if (nullptr == m_pSound_Manager)
		return -1;

	return m_pSound_Manager->Play_3DSound(strGroupKeys, strSoundKeys, eSoundGroup, pOwner, vOffsetPos, fVolume, ppOutChannel);
}

_int CGameInstance::Play_3DSound(vector<string> strGroupKeys, vector<string> strSoundKeys, ESoundGroup eSoundGroup
	, _float3 vPos, _float fVolume, _float f3DMin, _float f3DMax, FMOD_MODE eRollOffMode, FMOD::Channel** ppOutChannel)
{
	if (nullptr == m_pSound_Manager)
		return -1;

	return m_pSound_Manager->Play_3DSound(strGroupKeys, strSoundKeys, eSoundGroup, vPos, fVolume, f3DMin, f3DMax, eRollOffMode, ppOutChannel);
}

_bool CGameInstance::IsPlaying_Channel(_int* pChannelIndex, FMOD::Channel** ppChannel)
{
	if (nullptr == m_pSound_Manager)
		return false;

	return m_pSound_Manager->IsPlaying_Channel(pChannelIndex, ppChannel);
}

void CGameInstance::Sound_Move(_int* pChannelIndex, FMOD::Channel** ppChannel, _float3 vPos)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Sound_Move(pChannelIndex, ppChannel, vPos);
}

void CGameInstance::Sound_StopByChannel(_int* pChannelIndex, FMOD::Channel** ppChannel)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Sound_StopByChannel(pChannelIndex, ppChannel);
}

_int CGameInstance::Play_BGM(const wstring& strGroupKey, const wstring& strSoundKey, _float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return -1;

	return m_pSound_Manager->Play_BGM(strGroupKey, strSoundKey, fVolume);
}

_int CGameInstance::Play_BGM_LoopRange(const wstring& strGroupKey, const wstring& strSoundKey, _float fVolume, _uint iStart, _uint iEnd)
{
	if (nullptr == m_pSound_Manager)
		return -1;

	return m_pSound_Manager->Play_BGM_LoopRange(strGroupKey, strSoundKey, fVolume, iStart, iEnd);
}

void CGameInstance::FadeOut_BGM(_float fFadeOutSpeed)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->FadeOut_BGM(fFadeOutSpeed);
}

void CGameInstance::Stop_BGM()
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->Stop_BGM_All();
}

void CGameInstance::Stop_SoundAll()
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->Stop_All();
}

void CGameInstance::Set_ChannelVolume(_int eID, _float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->Set_ChannelVolume(eID, fVolume);
}

_float CGameInstance::Get_MasterVolume() const
{
	if (nullptr == m_pSound_Manager)
		return 0.f;

	return m_pSound_Manager->Get_MasterVolume();
}

void CGameInstance::Set_MasterVolume(_float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->Set_MasterVolume(fVolume);
}

_float CGameInstance::Get_BGMVolume() const
{
	if (nullptr == m_pSound_Manager)
		return 0.f;

	return m_pSound_Manager->Get_BGMVolume();
}

void CGameInstance::Set_BGMVolume(_float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->Set_BGMVolume(fVolume);
}

_float CGameInstance::Get_VoiceVolume() const
{
	if (nullptr == m_pSound_Manager)
		return 0.f;

	return m_pSound_Manager->Get_VoiceVolume();
}

void CGameInstance::Set_VoiceVolume(_float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->Set_VoiceVolume(fVolume);
}

_float CGameInstance::Get_EffectVolume() const
{
	if (nullptr == m_pSound_Manager)
		return 0.f;

	return m_pSound_Manager->Get_EffectVolume();
}

void CGameInstance::Set_EffectVolume(_float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->Set_EffectVolume(fVolume);
}

void CGameInstance::Set_ListenerAttributeByMatrix(_fmatrix Matrix)
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->Set_ListenerAttributeByMatrix(Matrix);
}

void CGameInstance::NormalModeDSP()
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->NormalModeDSP();
}

void CGameInstance::WaitModeDSP()
{
	if (nullptr == m_pSound_Manager)
		return;

	m_pSound_Manager->WaitModeDSP();
}

const _unmap<wstring, shared_ptr<FSoundData>>* CGameInstance::CRef_SoundMap() const
{
	if (nullptr == m_pSound_Manager)
		return nullptr;

	return &m_pSound_Manager->CRef_SoundMap();
}

vector<wstring> CGameInstance::Provide_GroupSoundNames(const wstring& strGroupKey) const
{
	if (nullptr == m_pSound_Manager)
		return vector<wstring>();
	
	return m_pSound_Manager->Provide_GroupSoundNames(strGroupKey);
}

#pragma endregion


#pragma region 사운드 포인트 매니저
HRESULT CGameInstance::Add_SoundPoint(const string& strName, const TSoundPointDesc& Desc, shared_ptr<class CSoundPoint>* ppOut)
{
	if (nullptr == m_pSoundPoint_Manager)
		return E_FAIL;

	return m_pSoundPoint_Manager->Add_SoundPoint(strName, Desc, ppOut);
}
HRESULT CGameInstance::Add_SoundPoint(const wstring& FilePath, vector<shared_ptr<class CSoundPoint>>* SoundPointVector)
{
	if (nullptr == m_pSoundPoint_Manager)
		return E_FAIL;

	return m_pSoundPoint_Manager->Add_SoundPoint(FilePath, SoundPointVector);
}
_bool CGameInstance::Exists_SoundPointName(const string& strName)
{
	if (nullptr == m_pSoundPoint_Manager)
		return false;

	return m_pSoundPoint_Manager->Exists_SoundPointName(strName);
}
HRESULT CGameInstance::Render_SoundPoints()
{
	if (nullptr == m_pSoundPoint_Manager)
		return E_FAIL;

	return m_pSoundPoint_Manager->Render();
}
void CGameInstance::Release_SoundPoint(shared_ptr<class CSoundPoint> pSoundPoint)
{
	if (nullptr == m_pSoundPoint_Manager)
		return;

	m_pSoundPoint_Manager->Release_SoundPoint(pSoundPoint);
}
void CGameInstance::Reset_SoundPoints()
{
	if (nullptr == m_pSoundPoint_Manager)
		return;

	m_pSoundPoint_Manager->Reset_SoundPoint();
}
void CGameInstance::Reset_SoundPointManager()
{
	if (nullptr == m_pSoundPoint_Manager)
		return;

	return m_pSoundPoint_Manager->Reset_SoundPoint();
}
shared_ptr<CSoundPoint> CGameInstance::Find_SoundPoint(const string& Name)
{
	if (nullptr == m_pSoundPoint_Manager)
		return nullptr;

	return m_pSoundPoint_Manager->Find_SoundPoint(Name);
}
#pragma endregion


#pragma region 쓰레드 풀
CThreadPool* CGameInstance::Get_ThreadPool()
{
	if (nullptr == m_pThreadPool)
		return nullptr;

	return m_pThreadPool;
}
#pragma endregion


#pragma region 절두체
void CGameInstance::Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix)
{
	if (nullptr == m_pFrustum)
		return;

	m_pFrustum->Transform_ToLocalSpace(WorldMatrix);
}

const BoundingFrustum* CGameInstance::Get_WorldFrustumPtr()
{
	if (nullptr == m_pFrustum)
		return nullptr;

	return m_pFrustum->Get_WorldFrustumPtr();
}
#pragma endregion



/* ETC */
#ifdef _DEBUG
void CGameInstance::Render_FPS(_cref_time fTimeDelta, HWND hwnd, wstring strLevelName )
{
	_float fps = 1.f / fTimeDelta;
	_tchar szBuf[64] = L"";
	swprintf_s(szBuf, L"Fps : %d", static_cast<int>(std::round(fps)));// 반올림
	strLevelName += szBuf;
	SetWindowText(hwnd, strLevelName.c_str());
	//m_pGameInstance->Render_Font(TEXT("Font_Default"), szBuf, _float2(0.f, 0.f));
}

#endif

XMFLOAT3 CGameInstance::RandomFloat3(XMFLOAT3 low, XMFLOAT3 high)
{
	uniform_real_distribution<float> RandomX(low.x, high.x);
	uniform_real_distribution<float> RandomY(low.y, high.y);
	uniform_real_distribution<float> RandomZ(low.z, high.z);

	return XMFLOAT3(RandomX(m_RandomNumber), RandomY(m_RandomNumber), RandomZ(m_RandomNumber));
}

XMFLOAT4 CGameInstance::RandomFloat4(XMFLOAT4 low, XMFLOAT4 high)
{
	uniform_real_distribution<float> RandomX(low.x, high.x);
	uniform_real_distribution<float> RandomY(low.y, high.y);
	uniform_real_distribution<float> RandomZ(low.z, high.z);
	uniform_real_distribution<float> RandomW(low.w, high.w);

	return XMFLOAT4(RandomX(m_RandomNumber), RandomY(m_RandomNumber), RandomZ(m_RandomNumber), RandomW(m_RandomNumber));
}

void CGameInstance::Random_Vector(vector<_float>& Out_values, _int num, _float low, _float high)
{
	Out_values.clear();
	Out_values.resize(num);
	uniform_real_distribution<_float> Random(low, high);
	auto generator = [&Random, this]() { return Random(m_RandomDevice); };
	std::generate(Out_values.begin(), Out_values.end(), generator);
}

void CGameInstance::RandomInt_Vector(vector<_int>& Out_values, _int num, _int low, _int high)
{
	Out_values.clear();
	Out_values.resize(num);
	uniform_int_distribution<_int> Random(low, high);
	auto generator = [&Random, this]() { return Random(m_RandomDevice); };
	std::generate(Out_values.begin(), Out_values.end(), generator);
}

void CGameInstance::RandomFloat3_Vector(vector<_float3>& Out_values, _int num, _float3 low, _float3 high)
{
	Out_values.clear();
	Out_values.resize(num);
	uniform_real_distribution<float> RandomX(low.x, high.x);
	uniform_real_distribution<float> RandomY(low.y, high.y);
	uniform_real_distribution<float> RandomZ(low.z, high.z);
	auto generator = [&RandomX, &RandomY, &RandomZ, this]() { return _float3(RandomX(m_RandomDevice), RandomY(m_RandomDevice), RandomZ(m_RandomDevice)); };
	std::generate(Out_values.begin(), Out_values.end(), generator);
}

string CGameInstance::RandomString(vector<string> Strings)
{
	_int iSize = (_int)Strings.size();
	if (iSize == 0)
		return "";

	uniform_int_distribution Random(0, iSize - 1);
	return Strings[Random(m_RandomDevice)];
}


/* Release */

void CGameInstance::Release_Manager()
{
	CUtility_File::Path_Mgr_Destroy();

	Safe_Release(m_pFrustum);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pTexture_Manager);
	Safe_Release(m_pModel_Manager);
	Safe_Release(m_pShader_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pThreadPool);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->Release_Manager();
	CPhysX_Manager::DestroyInstance();
}
