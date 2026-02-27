#ifdef _DEBUG
#pragma once
#include "Imgui_Tab.h"
#include "ImGuizmo.h"

BEGIN(Client)

class CImgui_Tab_ShaderEdit : public CImgui_Tab
{
private:
	CImgui_Tab_ShaderEdit(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	~CImgui_Tab_ShaderEdit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

private:
	weak_ptr<class CLight> m_pDirectionalLight;

	_float3 m_HSV = { 1.f,0.8f,0.7f };
	GFSDK_SSAO_Parameters mAOParameters;
	_bool m_bHBAO_Blur = { true };
	_bool m_bToneMapping = { true };
	_float4 m_vLight_Direction = {};
	_float4 m_vLight_Ambient = {};
	_float4 m_vLight_Diffuse = {};
	_float4 m_vLight_Specular = {};
	_bool	m_bHDRBloom = { true };
	_float	m_fHDRBloom_Brightness = { 1.1f };
	_float	m_fHDRBloom_Range = { 4.0f };
	_float	m_fHDRBloom_Stregth = { 1.0f };
	_float	m_fBlur_Range = { 4.0f };
	_float	m_fBlur_Stregth = { 2.0f };
	_bool  m_bAntiAliasing = { true };
	_bool  m_bPBR = { true };
	_float m_fExposure = { 1.0f };
	_bool		m_bMotionBlur = { false };
	_float		m_fMotionBlurScale = { 0.125f };
	_float2		m_vMotionBlurCenter = { 0.5f,0.5f };
	_float		m_fMotionBlurAmount = { 64.0f };
	_float		m_fFogStart = { 8.f };
	_float		m_fFogDensity = { 0.003f };
	_float		m_fFogFalloff = { 0.001f };
	_float		m_fFogSkyStrength = { 0.2f };
	_float4		m_vFogColor = { 0.1f,0.32f,0.28f,1.0f };
	_bool		m_bFogHeight = { true };
	_bool		m_bFog = { true };

	class CRenderer* m_pRenderer = nullptr;
public:
	static CImgui_Tab_ShaderEdit* Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void	Free() override;
};

END

#endif // DEBUG
