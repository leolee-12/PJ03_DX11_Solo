#include "stdafx.h"
#ifdef _DEBUG

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Imgui_Tab_ShaderEdit.h"
#include "ImGuiFileDialog.h"

#include "Imgui_Manager.h"

#include "VIBuffer_TerrainD.h"
#include "Level.h"
#include "Light.h"
#include "Renderer.h"

#include "Utility_File.h"

CImgui_Tab_ShaderEdit::CImgui_Tab_ShaderEdit(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CImgui_Tab(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Tab_ShaderEdit::Initialize()
{
	m_pRenderer = m_pGameInstance->Get_Renderer();
	m_HSV = m_pRenderer->Get_HSV();
	mAOParameters = m_pRenderer->Get_AOParameters();
	m_bHDRBloom = m_pRenderer->Get_HDRBloom();
	m_fHDRBloom_Brightness = m_pRenderer->Get_HDRBloom_Brightness();
	m_fHDRBloom_Range = m_pRenderer->Get_HDRBloom_Range();
	m_fHDRBloom_Stregth = m_pRenderer->Get_HDRBloom_Stregth();
	m_fBlur_Range = m_pRenderer->Get_Blur_Range();
	m_fBlur_Stregth = m_pRenderer->Get_Blur_Stregth();

	return S_OK;
}

void CImgui_Tab_ShaderEdit::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Tab_ShaderEdit::Render()
{
	if (ImGui::Button("Shader RecompileAll"))
	{
		//Batch 실행
		{
			wstring strPath = L"../../Bat/Copy.bat";			
			wchar_t strFilePath[MAX_PATH];
			GetFullPathName(strPath.c_str(), MAX_PATH, strFilePath, NULL);
			// CreateProcess 함수 사용
			{
				WCHAR ApplicationName[MAX_PATH];
				if (!GetEnvironmentVariable(L"comspec", ApplicationName, RTL_NUMBER_OF(ApplicationName)))
				{
					return GetLastError();
				}

				SIZE_T cch = wcslen(strFilePath);

				PWSTR CommandLine = (PWSTR)alloca(cch * sizeof(WCHAR) + sizeof(L"/c \"\""));

				swprintf(CommandLine, L"/c \"%s\"", strFilePath);

				PROCESS_INFORMATION procHandles;
				STARTUPINFO startWinInfo = { sizeof(startWinInfo) };

				if (CreateProcess(ApplicationName, CommandLine, NULL, NULL, 0,
					CREATE_DEFAULT_ERROR_MODE, NULL,
					NULL, &startWinInfo, &procHandles))
				{
					_int a = 0;
					CloseHandle(procHandles.hThread);

					switch (WaitForSingleObject(procHandles.hProcess, 3000))
					{
					case WAIT_OBJECT_0:
						break;
					default:
						TerminateProcess(procHandles.hProcess, 0);
					}
					CloseHandle(procHandles.hProcess);

				}
			}
		}
		m_pGameInstance->Recompile_EffectsAll();
	}


	ImGui::Checkbox("AntiAliasing_Enabled", &m_bAntiAliasing);
	m_pRenderer->Set_AntiAliasing(m_bAntiAliasing);

	ImGui::Checkbox("PBR_Enabled", &m_bPBR);
	m_pRenderer->Set_PBR(m_bPBR);

	m_pImgui_Manger->Arrow_Button("Exposure", 0.1f, &m_fExposure);
	m_pRenderer->Set_Exposure(m_fExposure);
	//ImGui::Separator();
	//if (m_pDirectionalLight.lock() == nullptr)
	//{
	//	m_pDirectionalLight = m_pGameInstance->Get_DirectionalLight();
	//	LIGHT_DESC Desc = m_pDirectionalLight.lock()->Get_LightDesc();
	//	m_vLight_Direction = Desc.vDirection;
	//	m_vLight_Diffuse = Desc.vDiffuse;
	//	m_vLight_Ambient = Desc.vAmbient;
	//	m_vLight_Specular = Desc.vSpecular;
	//}

	//ImGui::Text("Adjust DirectionalLight");
	//m_pImgui_Manger->InputFloat4("Direction", &m_vLight_Direction);
	//m_pImgui_Manger->InputFloat4("Diffuse", &m_vLight_Diffuse);
	//m_pImgui_Manger->InputFloat4("Ambient", &m_vLight_Ambient);
	//m_pImgui_Manger->InputFloat4("Specular", &m_vLight_Specular);

	//LIGHT_DESC Desc;
	//Desc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	//Desc.vDiffuse = m_vLight_Diffuse;
	//Desc.vAmbient = m_vLight_Ambient;
	//Desc.vSpecular = m_vLight_Specular;
	//Desc.vDirection = m_vLight_Direction;
	//m_pDirectionalLight.lock()->Set_LightDesc(Desc);

	ImGui::Text("Adjust HSV");

	m_pImgui_Manger->Arrow_Button("Hue", 0.1f, &m_HSV.x);
	m_pImgui_Manger->Arrow_Button("Saturation", 0.1f, &m_HSV.y);
	m_pImgui_Manger->Arrow_Button("Value", 0.1f, &m_HSV.z);

	m_pRenderer->Set_HSV(m_HSV);

	ImGui::Separator();

	ImGui::Text("Adjust HBAO");

	m_pImgui_Manger->Arrow_Button("Radius", 0.1f, &mAOParameters.Radius);
	m_pImgui_Manger->Arrow_Button("Bias", 0.1f, &mAOParameters.Bias);
	m_pImgui_Manger->Arrow_Button("PowerExponent", 0.1f, &mAOParameters.PowerExponent);
	ImGui::Checkbox("Blur_Enable", &m_bHBAO_Blur);
	m_pImgui_Manger->Arrow_Button("Blur_Sharpness", 0.1f, &mAOParameters.Blur.Sharpness);

	mAOParameters.Blur.Enable = m_bHBAO_Blur;
	m_pRenderer->Set_AOParameters(mAOParameters);

	ImGui::Separator();

	ImGui::Text("Adjust HDR");
	ImGui::Checkbox("ToneMapping_Enable", &m_bToneMapping);
	m_pRenderer->Set_ToneMapping(m_bToneMapping);
	ImGui::Checkbox("HDRBloom_Enable", &m_bHDRBloom);
	if (m_bHDRBloom)
	{
		m_pImgui_Manger->Arrow_Button("HDRBl_Brightness", 0.1f, &m_fHDRBloom_Brightness);
		m_pImgui_Manger->Arrow_Button("HDRBl_Range", 0.1f, &m_fHDRBloom_Range);
		m_pImgui_Manger->Arrow_Button("HDRBl_Stregth", 0.1f, &m_fHDRBloom_Stregth);
		m_pRenderer->Set_HDRBloom_Brightness(m_fHDRBloom_Brightness);
		m_pRenderer->Set_HDRBloom_Range(m_fHDRBloom_Range);
		m_pRenderer->Set_HDRBloom_Stregth(m_fHDRBloom_Stregth);
	}
	m_pRenderer->Set_HDRBloom(m_bHDRBloom);


	ImGui::Separator();
	ImGui::Text("Adjust EffectBlur");
	m_pImgui_Manger->Arrow_Button("Blur_Range", 0.1f, &m_fBlur_Range);
	m_pImgui_Manger->Arrow_Button("Blur_Stregth", 0.1f, &m_fBlur_Stregth);
	m_pRenderer->Set_Blur_Range(m_fBlur_Range);
	m_pRenderer->Set_Blur_Stregth(m_fBlur_Stregth);

	ImGui::Separator();
	ImGui::Text("Adjust MotionBlur");
	ImGui::Checkbox("MotionBlur_Enable", &m_bMotionBlur);
	m_pImgui_Manger->Arrow_Button("MotinoBlur_Scale", 0.1f, &m_fMotionBlurScale);
	m_pImgui_Manger->Arrow_Button("MotinoBlur_Amount", 0.1f, &m_fMotionBlurAmount);
	m_pImgui_Manger->InputFloat2("MotinoBlur_Center", &m_vMotionBlurCenter);
	m_pRenderer->Set_MotionBlur(m_bMotionBlur);
	m_pRenderer->Set_MotionBlurScale(m_fMotionBlurScale);
	m_pRenderer->Set_MotionBlurAmount(m_fMotionBlurAmount);
	m_pRenderer->Set_MotionBlurCenter(m_vMotionBlurCenter);

	ImGui::Separator();
	ImGui::Text("Adjust Fog");
	ImGui::Checkbox("Fog_Enable", &m_bFog);
	if (m_bFog)
	{
		ImGui::Checkbox("FogHeight_Enable", &m_bFogHeight);
		m_pImgui_Manger->Arrow_Button("Fog_Start", 1.f, &m_fFogStart);
		m_pImgui_Manger->Arrow_Button("Fog_Density", 0.0001f, &m_fFogDensity);
		m_pImgui_Manger->Arrow_Button("Fog_Falloff", 0.0001f, &m_fFogFalloff);
		m_pImgui_Manger->Arrow_Button("Fog_SkyStrength", 0.01f, &m_fFogSkyStrength);
		m_pImgui_Manger->InputFloat4("Fog_Color", &m_vFogColor);
		m_pRenderer->Set_FogHeight(m_bFogHeight);
		m_pRenderer->Set_FogStart(m_fFogStart);
		m_pRenderer->Set_FogDensity(m_fFogDensity);
		m_pRenderer->Set_FogFalloff(m_fFogFalloff);
		m_pRenderer->Set_FogSkyStrength(m_fFogSkyStrength);
		m_pRenderer->Set_FogColor(m_vFogColor);
	}
	m_pRenderer->Set_Fog(m_bFog);
	

	return S_OK;
}


CImgui_Tab_ShaderEdit* CImgui_Tab_ShaderEdit::Create(vector< shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CImgui_Tab_ShaderEdit* pInstance = new CImgui_Tab_ShaderEdit(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_ShaderEdit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Tab_ShaderEdit::Free()
{
	__super::Free();
}

#endif // DEBUG
