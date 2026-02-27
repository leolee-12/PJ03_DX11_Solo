#pragma once

#include "Base.h"
#include "Utility/DelegateTemplate.h"
/* 화면에 그려져야할 객체들을 그려야할 순서대로 모아놓는다. */
/* 모아놓은 순서대로 객체들의 렌더함수를 호출한다.(렌더콜) */

BEGIN(Engine)

#ifdef _DEBUG
typedef FastDelegate0<HRESULT> DelegateDebug;
typedef MulticastDelegate<DelegateDebug> MultiDeleDebug;
#endif

class ENGINE_DLL CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOW, RENDER_SHADOW_STATIC, RENDER_NONLIGHT, RENDER_NONBLEND, RENDER_NONBLEND_CHARACTER,  RENDER_BLEND_PARTICLE, RENDER_BLEND_EFFECT ,RENDER_BLEND, RENDER_UI, RENDER_END };

private:
	CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Initialize_HBAO();
	HRESULT Add_RenderGroup(RENDERGROUP eGroupID, shared_ptr<class CGameObject> pGameObject);
	HRESULT Draw_RenderGroup();

	HRESULT Add_DebugRender(shared_ptr<class CComponent> pDebugCom);

private:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	list<shared_ptr<class CGameObject>>				m_RenderObjects[RENDER_END];
	list<shared_ptr<class CComponent>>					m_DebugComponent;

private:
	shared_ptr<class CShader> m_pShader = { nullptr };
	shared_ptr<class CShader> m_pBlurShader = { nullptr };
	shared_ptr<class CVIBuffer_Rect> m_pVIBuffer = { nullptr };

	_float4x4								m_WorldMatrix;
	_float4x4								m_ViewMatrix, m_ProjMatrix;


	ComPtr<ID3D11DepthStencilView> m_pLightDepthDSV = { nullptr };
	D3D11_VIEWPORT m_ViewPortDesc[3] = {};

	RENDERTYPE m_eRenderType = { DEFAULT };

private:
	GFSDK_SSAO_Parameters				mAOParameters;
	GFSDK_SSAO_InputData_D3D11          mSSAOInputData;
	GFSDK_SSAO_Context_D3D11*			mSSAOContext;

private:
	ID3D11ShaderResourceView*			m_pDefaultSRV_ZERO = { nullptr };
	ID3D11ShaderResourceView*			m_pDefaultSRV_ONE = { nullptr };
	ID3D11ShaderResourceView*			m_pDefaultSRV_NORMAL = { nullptr };

private:
	shared_ptr<class CMaterialComponent> m_pMaterialCom_BRDF = { nullptr };
	shared_ptr<class CMaterialComponent> m_pMaterialCom_PreFiltered = { nullptr };
	shared_ptr<class CMaterialComponent> m_pMaterialCom_Irradiance = { nullptr };

private:

	_float4x4 m_Shadow_LightViewProjMatrices[4];
	_float4x4 m_Shadow_Matrices[4];
	_float m_fShadow_SplitDistances[4];
	_float m_fShadow_MapSize;
	ComPtr<ID3D11DepthStencilView> m_pShadowMapDSV = { nullptr };
	ComPtr<ID3D11ShaderResourceView> m_pShadowMapSRV = { nullptr };

private:
	_float		m_fSSRStep = 0.01f;
	_int		m_iSSRStepDistance = 50;

	_float3		m_vHSV = { 1.0f,0.8f,0.7f };
	_bool		m_bToneMapping = { true };
	_bool		m_bHDRBloom = { true };
	_float		m_fHDRBloom_Brightness = { 1.1f };
	_float		m_fHDRBloom_Range = { 4.0f };
	_float		m_fHDRBloom_Stregth = { 1.0f };
	_float		m_fBlur_Range = { 2.0f };
	_float		m_fBlur_Stregth = { 2.0f };
	_bool		m_bAntiAliasing = { true };
	_float4x4	m_AntiAliasingWorldMatrix;
	_float4x4	m_AntiAliasingProjMatrixTranspose;
	_bool		m_bPBR = { true };
	_float		m_fExposure = { 1.0f };
	_bool		m_bCascadeShadow = { true }; // 임시로 그림자 off
	_bool		m_bMotionBlur = { false };
	_float		m_fMotionBlurScale = { 0.125f };
	_float2		m_vMotionBlurCenter = { 0.5f,0.5f };
	_float		m_fMotionBlurAmount = { 64.0f };

	_float		m_bRadialBlur = { false };	// 추가
	_float3		m_vRadialBlurWorldPos = { 0.f,0.f,0.f }; //추가
	_float		m_fRadialBlurIntensity = { 0.5f }; // 추가

	_float		m_fFogStart = { 8.f };
	_float		m_fFogDensity = { 0.003f };
	_float		m_fFogFalloff = { 0.001f };
	_float		m_fFogSkyStrength = { 0.2f };
	_float4		m_vFogColor = { 0.1f,0.32f,0.28f,1.0f };
	_bool		m_bFogHeight = { true };
	_bool		m_bFog = { true };

	_bool		m_bShaderHalf = { false };
	_float		m_fShaderHalf_X = { 0.f };

public: 
	GETSET_EX2(_float3,m_vHSV,HSV,GET,SET)
	GETSET_EX2(GFSDK_SSAO_Parameters, mAOParameters, AOParameters, GET, SET)
	GETSET_EX2(_bool, m_bToneMapping, ToneMapping, GET, SET)
	GETSET_EX2(_bool, m_bHDRBloom, HDRBloom, GET, SET)
	GETSET_EX2(_float, m_fHDRBloom_Brightness, HDRBloom_Brightness, GET, SET)
	GETSET_EX2(_float, m_fHDRBloom_Range, HDRBloom_Range, GET, SET)
	GETSET_EX2(_float, m_fHDRBloom_Stregth, HDRBloom_Stregth, GET, SET)
	GETSET_EX2(_float, m_fBlur_Range, Blur_Range, GET, SET)
	GETSET_EX2(_float, m_fBlur_Stregth, Blur_Stregth, GET, SET)
	GETSET_EX2(_bool, m_bAntiAliasing, AntiAliasing, GET, SET)
	GETSET_EX2(_bool, m_bPBR, PBR, GET, SET)
	GETSET_EX2(_bool, m_bCascadeShadow, CascadeShadow, GET, SET)
	GETSET_EX2(_float, m_fExposure, Exposure, GET, SET)
	GETSET_EX2(_bool, m_bMotionBlur, MotionBlur, GET, SET)
	GETSET_EX2(_float, m_fMotionBlurScale, MotionBlurScale, GET, SET)
	GETSET_EX2(_float2, m_vMotionBlurCenter, MotionBlurCenter, GET, SET)
	GETSET_EX2(_float, m_fMotionBlurAmount, MotionBlurAmount, GET, SET)
	GETSET_EX2(_float, m_fFogStart, FogStart, GET, SET)
	GETSET_EX2(_float, m_fFogDensity, FogDensity, GET, SET)
	GETSET_EX2(_float, m_fFogFalloff, FogFalloff, GET, SET)
	GETSET_EX2(_float, m_fFogSkyStrength, FogSkyStrength, GET, SET)
	GETSET_EX2(_float4, m_vFogColor, FogColor, GET, SET)
	GETSET_EX2(_bool, m_bFog, Fog, GET, SET)
	GETSET_EX2(_bool, m_bFogHeight, FogHeight, GET, SET)

	GETSET_EX2(_bool, m_bRadialBlur, RadialBlur, GET, SET) // 추가
	GETSET_EX2(_float3, m_vRadialBlurWorldPos, RadialBlurWorldPos, GET, SET) // 추가
	GETSET_EX2(_float, m_fRadialBlurIntensity, RadialBlurIntensity, GET, SET) // 추가

	GETSET_EX2(ID3D11ShaderResourceView*, m_pDefaultSRV_ZERO,DefaultSRV_ZERO, GET, SET)
	GETSET_EX2(ID3D11ShaderResourceView*, m_pDefaultSRV_ONE,DefaultSRV_ONE, GET, SET)
	GETSET_EX2(ID3D11ShaderResourceView*, m_pDefaultSRV_NORMAL,DefaultSRV_NORMAL, GET, SET)

	GETSET_EX1(_float4x4*,m_Shadow_LightViewProjMatrices,Shadow_LightViewProjMatrices,GET)

public:
	void Set_RenderType(RENDERTYPE type) { m_eRenderType = type; }

private:
	_bool	Update_ShadowInfo();

	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonLight();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_Blend_Particle();
	HRESULT Render_Blend_Effect();
	HRESULT Render_Extract_Luminance();
	HRESULT Render_Bloom();
	HRESULT Render_BlendBloom();
	HRESULT Render_Result();
	HRESULT Render_MotionBlur();
	HRESULT Render_RadialBlur(); // 추가
	HRESULT Render_AntiAliasing();
	HRESULT Render_UI();

	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();
	HRESULT Render_RimLight();

	HRESULT Render_HBAO();
	HRESULT Render_SSR();


#ifdef _DEBUG
public:
	void Add_DebugEvent(DelegateDebug DebugEvent) { if (m_bIsDebugEvent) m_DebugEvent.Add(DebugEvent); }
	void Toggle_DebugDraw() { m_bIsDebugEvent = !m_bIsDebugEvent; }

private:
	HRESULT Render_Debug();
	HRESULT Render_PhysX_Debug();
	_bool m_bPhysXDebug = { true };
	_bool m_bIsDebugEvent = { true };
	MultiDeleDebug		m_DebugEvent;
#endif	

public:
	static CRenderer* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END