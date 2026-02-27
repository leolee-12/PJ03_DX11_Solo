#include "stdafx.h"
#ifdef _DEBUG
#include "Imgui_Tab_AnimOffset.h"

#include "Imgui_Manager.h"
#include "ImGuiFileDialog.h"

#include "AnimObject.h"
#include "CommonModelComp.h"
#include "AnimationComponent.h"
#include "BoneAnimContainer.h"
#include "GameInstance.h"
#include "CommonModelComp.h"
#include "BoneContainer.h"
#include "Model_Manager.h"


CImgui_Tab_AnimOffset::CImgui_Tab_AnimOffset(vector<shared_ptr<CGameObject>>* pGameObjectList, 
    ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : BASE(pGameObjectList, pDevice, pContext)
{
}

HRESULT CImgui_Tab_AnimOffset::Initialize()
{
	m_OffsetMatrix = XMMatrixIdentity();

    return S_OK;
}

void CImgui_Tab_AnimOffset::Tick(_cref_time fTimeDelta)
{
}

HRESULT CImgui_Tab_AnimOffset::Render()
{
	Use_ImGuizmo(m_OffsetMatrix, true);

    return S_OK;
}

CImgui_Tab_AnimOffset* CImgui_Tab_AnimOffset::Create(vector<shared_ptr<CGameObject>>* pGameObjectList, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	DERIVED* pInstance = new DERIVED(pGameObjectList, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImgui_Tab_AnimObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImgui_Tab_AnimOffset::Free()
{
	__super::Free();

}


#endif // _DEBUG
