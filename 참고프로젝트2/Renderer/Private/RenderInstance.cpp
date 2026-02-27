#include "RenderInstance.h"
#include "Target_Manager.h"
#include "GameInstance.h"
#include "Picking.h"

IMPLEMENT_SINGLETON(CRenderInstance)

CRenderInstance::CRenderInstance()
{

}

HRESULT CRenderInstance::Initialize_Engine(HWND hWnd, _bool isWindowed, _uint iNumLevels, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	//m_pLobbyRenderer = CLobby_Renderer::Create(*ppDevice, *ppContext);
	//if (nullptr == m_pLobbyRenderer)
	//	return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, hWnd);
	if (nullptr == m_pPicking)
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderInstance::Render_Engine(_float fTimeDelta)
{
	/* 엔진에서 관리하는 객체들 중, 반복적인 렌더가 필요한 객체들이 있다면. */
	/* 여기에서 렌더를 수행해준다. */

	if (FAILED(m_pRenderer->Draw(fTimeDelta)))
		return E_FAIL;

	//if (FAILED(m_pLobbyRenderer->Draw(fTimeDelta)))
	//	return E_FAIL;



	return S_OK;
}

HRESULT CRenderInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, CGameObject* pRenderObject, RENDER_OBJECT* pDesc)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderObject(eRenderGroup, pRenderObject, pDesc);
}

HRESULT CRenderInstance::Add_LobbyRenderObject(CLobby_Renderer::RENDERGROUP eRenderGroup, CGameObject* pRenderObject, RENDER_OBJECT* pDesc)
{
	if (nullptr == m_pLobbyRenderer)
		return E_FAIL;

	return m_pLobbyRenderer->Add_LobbyRenderObject(eRenderGroup, pRenderObject);
}

HRESULT CRenderInstance::Add_LobbyDebugComponent(CComponent* pDebugComponent)
{
	if (nullptr == m_pLobbyRenderer)
		return E_FAIL;

	return m_pLobbyRenderer->Add_LobbyDebugComponent(pDebugComponent);
}

HRESULT CRenderInstance::Add_DebugComponent(CComponent* pDebugComponent)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_DebugComponent(pDebugComponent);
}

void CRenderInstance::SetActive_RenderTarget(_bool isOn)
{
	m_pRenderer->SetActive_RenderTarget(isOn);
}

void CRenderInstance::SetActive_Debug_Component(_bool isOn)
{
	m_pRenderer->SetActive_Debug_Component(isOn);
}

void CRenderInstance::Create_Distortion(DISTORTION_DESC& tDistortionDesc)
{
	m_pRenderer->Create_Distortion(tDistortionDesc);
}

void CRenderInstance::Delete_LoopDistortion()
{
	m_pRenderer->Delete_LoopDistortion();
}

void CRenderInstance::Set_AuraColor(_float4 vColor)
{
	m_pRenderer->Set_AuraColor(vColor);
}

void CRenderInstance::Create_HitDistortion(_float4 vPlayerPos, _float3 vDir, _float2 vOffSetPos, _float2 vOffSetScale, _float fLifeTime)
{
	m_pRenderer->Create_HitDistortion(vPlayerPos, vDir, vOffSetPos, vOffSetScale, fLifeTime);
}

void CRenderInstance::Show_OutLine()
{
	m_pRenderer->Show_OutLine();
}

void CRenderInstance::Show_Layer_View()
{
	m_pRenderer->Show_Layer_View();
}

_bool CRenderInstance::Get_isLayerView()
{
	return m_pRenderer->Get_isLayerView();
}

void CRenderInstance::Set_CurMapType(CRenderer::MAP_TYPE eType)
{
	m_pRenderer->Set_CurMapType(eType);
}

HRESULT CRenderInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CRenderInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CRenderInstance::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV);
}

HRESULT CRenderInstance::Begin_MRT_DoNotClear(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT_DoNotClear(strMRTTag, pDSV);
}

HRESULT CRenderInstance::Begin_EffectMRT(const _wstring& strMRTTag, _uint iArrayIndex, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_EffectMRT(strMRTTag, iArrayIndex, pDSV);
}

HRESULT CRenderInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CRenderInstance::Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D)
{
	return m_pTarget_Manager->Copy_RenderTarget(strTargetTag, pTexture2D);
}

ID3D11ShaderResourceView* CRenderInstance::Copy_RenderTarget_SRV(const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Copy_RenderTarget_SRV(strTargetTag);
}

HRESULT CRenderInstance::Bind_RT_ShaderResource(CShader* pShader, const _char* pConstantName, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Bind_ShaderResource(pShader, pConstantName, strTargetTag);
}

HRESULT CRenderInstance::Bind_RT_EffectShaderResource(CShader* pShader, const _char* pConstantName, const _wstring& strTargetTag, _uint isPri)
{
	return m_pTarget_Manager->Bind_EffectShaderResource(pShader, pConstantName, strTargetTag, isPri);
}

_int CRenderInstance::Get_FrameGlowCount(_int isPri)
{
	return m_pTarget_Manager->Get_FrameGlowCount(isPri);
}

_float CRenderInstance::Get_CulGlowIndex(_int isPri)
{
	return m_pTarget_Manager->Get_CulGlowIndex(isPri);
}

void CRenderInstance::Zero_CurGlowIndex(_int isPri)
{
	return m_pTarget_Manager->Zero_CurGlowIndex(isPri);
}

_int CRenderInstance::Add_ClientRenderTarget(const _wstring& strMRTTag, const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor)
{
	return m_pTarget_Manager->Add_ClientRenderTarget(strMRTTag, strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CRenderInstance::Add_ClientRenderTargetToMRT(const _wstring& strMRTTag, const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor)
{
	return m_pTarget_Manager->Add_ClientRenderTargetToMRT(strMRTTag, strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

#ifdef _DEBUG
HRESULT CRenderInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fCenterX, _float fCenterY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fCenterX, fCenterY, fSizeX, fSizeY);
}

HRESULT CRenderInstance::Render_RT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif

LIGHT_DESC* CRenderInstance::Get_LightDesc(CLight_Manager::LIGHT_TYPE eLightType, _uint iLightIndex, string strName)
{
	return m_pLight_Manager->Get_LightDesc(eLightType, iLightIndex, strName);
}

_int CRenderInstance::Check_EffectLights()
{
	return m_pLight_Manager->Check_EffectLights();
}

void CRenderInstance::Remove_LightDesc(string strName)
{
	return m_pLight_Manager->Remove_LightDesc(strName);
}

HRESULT CRenderInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

HRESULT CRenderInstance::Add_Player_Light(string strKey, const LIGHT_DESC& LightDesc, _float4 vChaseColor, _bool* pisChaseLight)
{
	return m_pLight_Manager->Add_Player_Light(strKey, LightDesc, vChaseColor, pisChaseLight);
}

HRESULT CRenderInstance::Add_Effect_Light(string strKey, const LIGHT_DESC& LightDesc)
{
	return  m_pLight_Manager->Add_Effect_Light(strKey, LightDesc);
}

void CRenderInstance::BGLight_Pop_Front()
{
	m_pLight_Manager->BGLight_Pop_Front();
}

HRESULT CRenderInstance::Render_Lights(CLight_Manager::LIGHT_TYPE eLightType, CShader* pShader, CVIBuffer_Rect* pVIBuffer, const string strName,_float fTimeDelta)
{
	return m_pLight_Manager->Render_Lights(eLightType, pShader, pVIBuffer, strName, fTimeDelta);
}

void CRenderInstance::Clear_Light()
{
	m_pLight_Manager->Clear_Light();
}

_float4 CRenderInstance::Picked_Position(_bool* pPicked)
{
	return m_pPicking->Picked_Position(pPicked);
}

_int CRenderInstance::Picked_Effect_Index()
{
	return m_pPicking->Picked_Effect_Index();
}

void CRenderInstance::Switch_BlackOut(_bool isTrue)
{
	m_pRenderer->Switch_BlackOut(isTrue);
}

void CRenderInstance::Switch_AllBlackOut()
{
	m_pRenderer->Switch_AllBlackOut();
}

void CRenderInstance::Start_WhiteOut(_float2 vDir, _bool* isDone, _float fWhiteSpeed)
{
	m_pRenderer->Start_WhiteOut(vDir, isDone, fWhiteSpeed);
}

void CRenderInstance::Start_AllWhiteOut(_float fWhiteTime, _float fSpeed)
{
	m_pRenderer->Start_AllWhiteOut(fWhiteTime, fSpeed);
}


void CRenderInstance::Release_Engine()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pLobbyRenderer);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pPicking);

	CRenderInstance::Get_Instance()->Destroy_Instance();
}

void CRenderInstance::Free()
{
	__super::Free();
}
