#pragma once
#include "GameObject.h"
#include "Prototype_Manager.h"

/* ------------------------------------------------------------ */
// CGameInstance : 클라이언트에게 엔진 기능을 위한 메서드 제공
/* ------------------------------------------------------------ */

NS_BEGIN(Engine)
class CLevel;

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
#pragma region ENGINE
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Begin_Draw();
	HRESULT Draw();
	HRESULT End_Draw();
	void Clear_Resources(_int iLevelIndex);
	void Release_Engine();
	HRESULT Resize_Engine(_uint iNewWidth, _uint iNewHeight);
	HRESULT Resize_Surface(_uint iNewWidth, _uint iNewHeight);

	const HWND Get_HWND() const;
	_float Random(_float fMin, _float fMax);
	_float2 Get_ViewportSize() { return m_vViewportSize; }
	void Toggle_Debug() { m_bDebug = !m_bDebug; }
	_bool Is_Debug() { return m_bDebug; }

	ID3D11Device* Get_Device() const;
	ID3D11DeviceContext* Get_Context() const;

#ifdef _DEBUG
	_float Get_DebugRendererMS() const { return m_fDebugRendererMS; }
	_float Get_DebugLevelRenderMS() const { return m_fDebugLevelRenderMS; }
	_float Get_DebugDrawMS() const { return m_fDebugDrawMS; }
#endif
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(WNameID strTimerTag);
	float Compute_Timer(WNameID strTimerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_int iNewLevelIndex, class CLevel* pNewLevel);
	HRESULT Push_Level(_int iLevelIndex, class CLevel* pNewLevel);
	HRESULT Pop_Level();
	_int    Get_CurrentLevel() const;
	CLevel* Get_CurrentLevelPtr() const;
	_bool   Is_Level_Active(_uint iLevel) const;
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, WNameID strProtoTag, CBase* pPrototype);
	_bool Has_Prototype(_uint iLevelIndex, WNameID strProtoTag);
	CBase* Clone_Prototype(PROTOTYPE eType, _uint iLevelIndex, WNameID strProtoTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
	class CComponent* Get_Component(_uint iLevelIndex, const WNameID strLayerTag, const WNameID strComponentTag, _uint iIndex = 0);
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, WNameID strProtoTag, _uint iLayerLevelIndex, WNameID strLayerTag, void* pArg = nullptr);
	HRESULT Add_GameObject_Ex(_uint iLayerLevel, WNameID strLayerTag, CGameObject* pObj);
	const list<class CGameObject*>* Get_ObjectList(_uint iLevel, WNameID strLayerTag);
	vector<CGameObject*> Get_LevelObjects(_uint iLevel) const;
#pragma endregion

#pragma region RENDERER
	void Add_RenderGroup(RENDERID eGroupID, class CGameObject* pGameObject);
	void Set_UseShadow(_bool b);
	void Set_OutlineParam(const OUTLINE_PARAM& Param);
#ifdef _DEBUG
	void Add_DebugComponent(class CComponent* pComponent);
#endif
#pragma endregion

#pragma region PIPELINE
	const _float4x4* Get_Transform(D3DTS eState) const;
	const _float4x4* Get_Transform_Inverse(D3DTS eState) const;
	const _float4* Get_CamPosition() const;

	void XM_CALLCONV Set_CameraWorld(_fmatrix StateMatrix);
	void XM_CALLCONV Set_Projection(_fmatrix StateMatrix);

	void Set_MainCamera(class CCamera* pCamera);
	class CCamera* Get_MainCamera() const;
	const _float* Get_FarZPtr() const;
	void Toggle_CameraFollow();
#pragma endregion

#pragma region INPUT_DEVICE
	_float2 Get_CursorClientF() const;
	_bool   Is_Cursor_InClient() const;

	void Set_InputState(INPUT_STATE eState);
	INPUT_STATE Get_InputState() const;

	_bool Key_Down(_ubyte byKeyID);
	_bool Key_Up(_ubyte byKeyID);
	_bool Key_Pressing(_ubyte byKeyID);

	_bool Mouse_Down(DIMB eMouse);
	_bool Mouse_Up(DIMB eMouse);
	_bool Mouse_Pressing(DIMB eMouse);
	_long Mouse_Move(DIMM eMouseState);
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Light(CShader* pShader, CVIBuffer_Rect* pVIBuffer);
	void Clear_Lights();
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const WNameID strFontTag, const _tchar* pFontFilePath);
	_float2 Measure_Text(const WNameID strFontTag, const _tchar* pText);
	HRESULT XM_CALLCONV Draw_Text(const WNameID strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), const _float2& vScale = _float2(1.f, 1.f));
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(WNameID strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(WNameID strMRTTag, WNameID strTargetTag);
	HRESULT Begin_MRT(WNameID strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(WNameID strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RT_Resource(WNameID strTargetTag, ID3D11Texture2D* pOut);
	HRESULT Copy_RT_SubResource(WNameID strTargetTag, ID3D11Texture2D* pOut, D3D11_BOX* pSrcBox);
#ifdef _DEBUG
	HRESULT Ready_RT_Debug(WNameID strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RT_Debug(WNameID strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region PICKING
	_bool Picking(_float4& Out);
#pragma endregion

#pragma region SHADOW
	const _float4x4* Get_Shadow_Transform(D3DTS eState) const;
	HRESULT Set_ShadowLight(const SHADOW_LIGHT_DESC& ShadowDesc);
	HRESULT Bind_Shadow_FarZ(CShader* pShader);
#pragma endregion

#pragma region SHARED_TEXTURE_BINDER
	void Set_SharedTextureBinder(class ISharedTextureBinder* pBinder) { m_pSharedTextureBinder = pBinder; }
	class ISharedTextureBinder* Get_SharedTextureBinder() const { return m_pSharedTextureBinder; }
#pragma endregion

#pragma region SOUND_MANAGER
	HRESULT Play(const _tchar* pSoundKey, CHANNELID eChannelID = CHANNELID::SFX, _float fVolume = 1.f, _bool bLoop = false);
	HRESULT Play_BGM(const _tchar* pSoundKey, _float fVolume = 1.f);
	HRESULT Play_3D(const _tchar* pSoundKey, const _float3& vPosition, CHANNELID eChannelID = CHANNELID::SFX,
		_float fVolume = 1.f, _float fMinDistance = 1.f, _float fMaxDistance = 100.f, _bool bLoop = false);

	void Stop_Sound(CHANNELID eChannelID);
	void Stop_Group(CHANNELID eChannelID);
	void Stop_All();

	void Set_ChannelVolume(CHANNELID eChannelID, _float fVolume);
	void Set_GroupVolume(CHANNELID eChannelID, _float fVolume);
#pragma endregion

#pragma region FRUSTUM
	void XM_CALLCONV Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix);
	_bool XM_CALLCONV isIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool isIn_Frustum_WorldSpace_AABB(const _float3* pWorldCorners, _uint iNumCorners);
	_bool XM_CALLCONV isIn_Frustum_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);
#pragma endregion

private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CInput_Device*		m_pInput_Device = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };
	class CFont_Manager*		m_pFont_Manager = { nullptr };
	class CTarget_Manager*		m_pTarget_Manager = { nullptr };
	class CPicking*				m_pPicking = { nullptr };
	class CShadow*				m_pShadow = { nullptr };
	class CFrustum*				m_pFrustum = { nullptr };

	class CCamera*				m_pMainCamera = { nullptr };
	class ISharedTextureBinder* m_pSharedTextureBinder = { nullptr };
	class CSound_Manager*		m_pSound_Manager = { nullptr };

	_float2						m_vViewportSize{};
	_bool						m_bDebug = { false };
	_float						m_fDummy = { 500.f };

#ifdef _DEBUG
	_float m_fDebugRendererMS = { 0.f };
	_float m_fDebugLevelRenderMS = { 0.f };
	_float m_fDebugDrawMS = { 0.f };
#endif

private:
	virtual void Free() override;
};

NS_END
