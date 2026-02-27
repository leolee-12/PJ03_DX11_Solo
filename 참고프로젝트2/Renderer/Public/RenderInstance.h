#pragma once

#include "Renderer.h"
#include "Lobby_Renderer.h"
#include "Light_Manager.h"
BEGIN(Engine)
class CGameInstance;
END

BEGIN(Renderer)

class RENDERER_DLL CRenderInstance final : public CBase
{
	DECLARE_SINGLETON(CRenderInstance)
private:
	CRenderInstance();
	virtual ~CRenderInstance() = default;

public:
	/* 엔진을 초기화한다. */
	HRESULT Initialize_Engine(HWND hWnd, _bool isWindowed, _uint iNumLevels, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	HRESULT Render_Engine(_float fTimeDelta);

public: /* For.Renderer */
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject, RENDER_OBJECT* pDesc = nullptr);
	HRESULT Add_DebugComponent(class CComponent* pDebugComponent);
	HRESULT Add_LobbyRenderObject(CLobby_Renderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject, RENDER_OBJECT* pDesc = nullptr);
	HRESULT Add_LobbyDebugComponent(class CComponent* pDebugComponent);
	void SetActive_RenderTarget(_bool isOn);
	void SetActive_Debug_Component(_bool isOn);
	void Create_Distortion(DISTORTION_DESC& tDistortionDesc);
	void Delete_LoopDistortion();
	
	void Set_AuraColor(_float4 vColor);
	/* 캐릭터 1Team 기준 : 가만히 있을때 오른쪽으로 순간이동 시작 하는 느낌 Dir은 x -1 
	왼쪽에서 오른쪽으로 도착했을때 느낌 Dir은 x 1 */
	void Create_HitDistortion(_float4 vPlayerPos, _float3 vDir = { 1,0,0 }, _float2 vOffSetPos = { 0.f,0.f }, _float2 vOffSetScale = { 1.f,1.f }, _float fLifeTime = 0.1f);
	void Show_OutLine();
	void Show_Layer_View();
	_bool Get_isLayerView();
	void Set_CurMapType(CRenderer::MAP_TYPE eType);

public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT Begin_MRT_DoNotClear(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT Begin_EffectMRT(const _wstring& strMRTTag, _uint iArrayIndex = 2, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D);
	ID3D11ShaderResourceView* Copy_RenderTarget_SRV(const _wstring& strTargetTag);
	HRESULT Bind_RT_ShaderResource(class CShader* pShader, const _char* pConstantName, const _wstring& strTargetTag);
	HRESULT Bind_RT_EffectShaderResource(class CShader* pShader, const _char* pConstantName, const _wstring& strTargetTag, _uint isPri);
	
	_int	Get_FrameGlowCount(_int isPri);
	_float	Get_CulGlowIndex(_int isPri);
	void	Zero_CurGlowIndex(_int isPri);

	_int Add_ClientRenderTarget(const _wstring& strMRTTag, const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor);
	HRESULT Add_ClientRenderTargetToMRT(const _wstring& strMRTTag, const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _fvector vClearColor);
	
#ifdef _DEBUG
public:
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fCenterX, _float fCenterY, _float fSizeX, _float fSizeY);
	HRESULT Render_RT_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

public:/*For.Light_Manager*/
	LIGHT_DESC* Get_LightDesc(CLight_Manager::LIGHT_TYPE eLightType, _uint iLightIndex, string strName = "");
	_int		Check_EffectLights();
	void		Remove_LightDesc(string strName);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Add_Player_Light(string strKey, const LIGHT_DESC& LightDesc, _float4 vChaseColor = { 1.f,1.f,1.f,1.f }, _bool* pisChaseLight = { nullptr });
	HRESULT Add_Effect_Light(string strKey, const LIGHT_DESC& LightDesc);
	void	BGLight_Pop_Front();
	HRESULT Render_Lights(CLight_Manager::LIGHT_TYPE eLightType, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer,const string strName, _float fTimeDelta);
	void Clear_Light();
public:/*For.Picking*/
	_float4 Picked_Position(_bool* pPicked);
	_int Picked_Effect_Index();
public: /* For.Renderer */
	void Switch_BlackOut(_bool isTrue);
	void Switch_AllBlackOut();
	void Start_WhiteOut(_float2 vDir, _bool* isDone, _float fWhiteSpeed = 1.f);
	void Start_AllWhiteOut(_float fWhiteTime, _float fSpeed);
private:
	class CRenderer*		m_pRenderer = { nullptr };
	class CLobby_Renderer* m_pLobbyRenderer = { nullptr };
	class CTarget_Manager*	m_pTarget_Manager = { nullptr };
	class CLight_Manager*	m_pLight_Manager = { nullptr };
	class CPicking*			m_pPicking = { nullptr };
public:
	void Release_Engine();

	virtual void Free() override;
};

END

