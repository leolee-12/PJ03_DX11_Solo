#include "Game_API.h"
#include "GameInstance.h"
#include "Level_Loading.h"

NS_BEGIN(Game_PKM)

HRESULT Ready_Prototype_For_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameInstance* m_pGameInstance = CGameInstance::GetInstance();

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), WName(L"Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(pDevice, pContext))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOUI(LEVEL::STATIC), WName(L"Prototype_Component_Shader_VtxTex"),
		CShader::Create(pDevice, pContext, TEXT("../../ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	return S_OK;
}

HRESULT Start_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eStartLevelID)
{
	CLevel* pPreLevel = CLevel_Loading::Create(pDevice, pContext, eStartLevelID);

	if (nullptr == pPreLevel)
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Change_Level(ETOI(LEVEL::LOADING), pPreLevel)))
		return E_FAIL;

	return S_OK;
}

NS_END