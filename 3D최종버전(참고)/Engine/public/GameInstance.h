#pragma once

#include "Prototype_Manager.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

#pragma region ENGINE
public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Begin_Draw(const _float4* pColor);
	HRESULT Draw();
	HRESULT End_Draw();
	void Clear_Resources(_uint iLevelID);

	_float Random(_float fMin, _float fMax);
#pragma endregion

#pragma region INPUT_DEVICE
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);	
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float Get_TimeDelta(const _wstring& pTimerTag);
	HRESULT Add_Timer(const _wstring& pTimerTag);
	void Update_TimeDelta(const _wstring& pTimerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_uint iLevelID, class CLevel* pNewLevel);
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iLevelID, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase* Clone_Prototype(PROTOTYPE eType, _uint iLevelID, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER

	class CComponent* Get_Component(_uint iLayerLevelID, const _wstring& strLayerTag, _uint iIndex, const _wstring& strComponentTag);
	HRESULT Add_GameObject(_uint iPrototypeLevelID, const _wstring& strPrototypeTag, _uint iLayerLevelID, const _wstring& strLayerTag, void* pArg = nullptr);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDERGROUP eGroupID, class CGameObject* pRenderObject);	
#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pDebugComponent);
#endif
#pragma endregion

#pragma region PIPELINE
	const _float4x4* Get_Transform_Float4x4_Ptr(D3DTS eState);
	_matrix Get_Transform_Matrix(D3DTS eState);
	const _float4x4* Get_Transform_Float4x4_Inverse_Ptr(D3DTS eState);
	_matrix Get_Transform_Matrix_Inverse(D3DTS eState);
	const _float4* Get_CamPosition();
	void Set_Transform(D3DTS eState, _fmatrix TransformMatrix);
#pragma endregion

#pragma region LIGHT_MANGER
	const LIGHT_DESC* Get_Light(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#pragma endregion

#pragma region FONT_MANGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Draw_Font(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f));
	
#pragma endregion


#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, _bool isClearTarget = true, _bool isClearDepth = false, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RT_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pOut);
#ifdef _DEBUG
public:
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

#pragma endregion

#pragma region SHADOW
	HRESULT Add_Shadow_Light(const SHADOW_DESC& ShadowDesc);
	const _float4x4* Get_Shadow_Transform_Float4x4_Ptr(D3DTS eState);
#pragma endregion

#pragma region PICKING
	_bool Picking(_float3* pOut);
#pragma endregion

#pragma region FRUSTUM	
	void Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool isIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRadius = 0.f);
	_bool isIn_Frustum_LocalSpace(_fvector vLocalPos, _float fRadius = 0.f);
#pragma endregion


	


private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CInput_Device*		m_pInput_Device = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };
	class CFont_Manager*		m_pFont_Manager = { nullptr };
	class CTarget_Manager*		m_pTarget_Manager = { nullptr };
	class CShadow*				m_pShadow = { nullptr };
	class CPicking*				m_pPicking = { nullptr };
	class CFrustum*				m_pFrustum = { nullptr };


public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END